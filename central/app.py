#!/usr/bin/python3

import sys,os,signal
import time
from threading import Thread

import curses
from curses.textpad import rectangle

import socket
import json


from andar import Andar
from inout import InOut


firstKey = curses.KEY_F1
keyboardPress = -1

alarmeAtivado = True
andares = []


inputsValidos = ['presenca', 'fumaca', 'janela', 'contagem', 'porta', 'dth22']
outputsValidos = ['lampada', 'ar-condicionado', 'aspersor']


def pintarInterface(stdscr):
    global alarmeAtivado, andares, inputsValidos, outputsValidos, keyboardPress

    stdscr.clear()
    stdscr.refresh()
    stdscr.nodelay(True)

    curses.start_color()
    curses.init_pair(1, curses.COLOR_CYAN, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_BLACK)
    curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)
    curses.init_pair(4, curses.COLOR_MAGENTA, curses.COLOR_BLACK)
    curses.init_pair(5, curses.COLOR_GREEN, curses.COLOR_GREEN)
    curses.curs_set(0)

    while(keyboardPress != curses.ascii.ESC):

        stdscr.erase()
        height, width = stdscr.getmaxyx()

        title = f"Servidor Central -> {sys.argv[1]}:{sys.argv[2]}"[:width-1]
        title_x = int((width // 2) - (len(title) // 2) - len(title) % 2)
        stdscr.addstr(0, title_x, title, curses.color_pair(1))

        footer = "Pressione 'Esc' para sair"
        stdscr.attron(curses.color_pair(3))
        stdscr.addstr(height-1, 0, footer)
        stdscr.addstr(height-1, len(footer), " " * (width - len(footer) - 1))
        stdscr.attroff(curses.color_pair(3))

        mid_x = width // 2
        subbar_h = 10
        rectangle_h = height - subbar_h
        margin_x = 2
        line = 3

        stdscr.attron(curses.color_pair(4))
        rectangle(stdscr, 1, 0, rectangle_h, mid_x)
        rectangle(stdscr, 1, mid_x, rectangle_h, width-1)
        rectangle(stdscr, rectangle_h+1, 0, rectangle_h+subbar_h-2, width-1)
        stdscr.attron(curses.color_pair(4))

        if(len(andares) >= 1):
            titleCol1 = f"Conectado: {andares[0].getNome()}"
            andart = andares[0].getContagem().getValue() if andares[0].getContagem() != None else ''
            stdscr.addstr(2, margin_x, f"Temperatura: {andares[0].getTemperatura():.1f} Cº", curses.color_pair(2))
            stdscr.addstr(3, margin_x, f"Humidade: {andares[0].getHumidade():.1f} %", curses.color_pair(2))
            pintarIOInterface(stdscr, line+2, margin_x, andares[0].getInputs())
            pintarIOInterface(stdscr, line+4+len(andares[0].getOutputs()), margin_x, andares[0].getOutputs())
            stdscr.addstr(rectangle_h+2, margin_x, f"Pessoas no prédio: {andart}", curses.color_pair(2))
        else:
            titleCol1 = f"Sem conexão!"
        stdscr.addstr(1, margin_x, titleCol1)

        if(len(andares) >= 2):
            titleCol2 = f"Conectado: {andares[1].getNome()}"
            andar1 = andares[1].getContagem().getValue() if andares[1].getContagem() != None else ''
            stdscr.addstr(2, mid_x+margin_x, f"Temperatura: {andares[1].getTemperatura():.1f} Cº", curses.color_pair(2))
            stdscr.addstr(3, mid_x+margin_x, f"Humidade: {andares[1].getHumidade():.1f} %", curses.color_pair(2))
            pintarIOInterface(stdscr, line+2, mid_x+margin_x, andares[1].getInputs())
            pintarIOInterface(stdscr, line+4+len(andares[1].getOutputs()), mid_x+margin_x, andares[1].getOutputs())
            stdscr.addstr(rectangle_h+3, margin_x, f"Pessoas no {andares[0].getNome()}: {andart-andar1}", curses.color_pair(2))
            stdscr.addstr(rectangle_h+4, margin_x, f"Pessoas no {andares[1].getNome()}: {andar1}", curses.color_pair(2))
        else:
            titleCol2 = f"Sem conexão!"
        stdscr.addstr(1, margin_x+mid_x, titleCol2)

        stdscr.addstr(rectangle_h+1, margin_x, "Contador de pessoas")

        stdscr.refresh()
        keyboardPress = stdscr.getch()

        time.sleep(0.01)


def pintarIOInterface(stdscr, lineStart: int, margin_x: int, ios: list):
    for tmp_in in ios:
        if(tmp_in.getKey() != None):
            press = f"F{tmp_in.getKey() % curses.KEY_F0}"
            stdscr.addstr(lineStart, margin_x, f"{press}", curses.color_pair(3))
        else:
            press = ''

        stdscr.addstr(lineStart, margin_x+len(press), f"{tmp_in.getTag()}: {tmp_in.getValue()}", curses.color_pair(2))
        lineStart += 1


def lerSocket(con):
    global alarmeAtivado, andares

    andar = None
    while True:
        msg = con.recv(1024)
        if not msg: break
        msgs = [tmp+'}' for tmp in msg.decode('utf8').split('}') if tmp]

        for tmp in msgs:
            json_object = json.loads(tmp)
            if(json_object['mode'] == 'create'):
                if(json_object['type'] == "nome"):
                    andar = Andar(json_object['tag'])
                    if(json_object['tag'] == 'Térreo'):
                        andares.insert(0, andar)
                    else:
                        andares.append(andar)
                else:
                    criarIO(json_object, andar)

            elif(json_object['mode'] == 'update'):
                atualizarInput(json_object, andar)

    con.close() 


def enviarSocket(con):
    global keyboardPress, andares

    gpio = -1
    value = -1
    while True:
        if(keyboardPress == -1):
            time.sleep(0.01)
            continue
        elif(keyboardPress == curses.ascii.ESC):
            break
        elif(keyboardPress == ord('a')):
            ativarAlarme()

        for tmp in andares:
            if(keyboardPress == -1): break
            for tmp_io in tmp.getOutputs():
                if(tmp_io.getKey() == keyboardPress):
                    gpio = tmp_io.getGpio()
                    value = not tmp_io.getValue()
                    tmp_io.setValue(value)
                    keyboardPress = -1
                    break  
        keyboardPress = -1

        if(gpio == -1 and value == -1): continue
        buff = {"gpio":gpio,"value":value}
        nsent = con.send(json.dumps(buff).encode('utf-8'))
        if not nsent: break


def criarIO(json_object, andar):
    global andares, inputsValidos, outputsValidos, firstKey
    msgIo = InOut(json_object['type'], json_object['tag'], json_object['gpio'], json_object['value'])

    if(andar == None):
        pass

    elif json_object['type'] in inputsValidos:
        for i, tmp in enumerate(andares):
            if(tmp.getNome() == andar.getNome()):
                if(json_object['type'] == 'contagem'):
                    andares[i].setContagem(msgIo)
                else:
                    andares[i].addInput(msgIo)

    elif json_object['type'] in outputsValidos:
        for i, tmp in enumerate(andares):
            if(tmp.getNome() == andar.getNome()):
                msgIo.setKey(firstKey)
                firstKey += 1
                andares[i].addOutput(msgIo)


def atualizarInput(json_object, andar):
    global andares, inputsValidos
    if(andar == None):
        pass

    elif json_object['type'] in inputsValidos:
        for i, tmp in enumerate(andares):
            if(tmp.getNome() == andar.getNome()):
                if(json_object['type'] == 'dth22'):
                    andares[i].setTemperatura(json_object['temperatura'])
                    andares[i].setHumidade(json_object['humidade'])
                elif(json_object['type'] == 'contagem'):
                    andares[i].getContagem().setValue(json_object['value'])
                else:
                    for j, in_tmp in enumerate(andares[i].getInputs()):
                        if(in_tmp.getGpio() == json_object['gpio']):
                            andares[i].getInputs()[j].setValue(json_object['value'])


def initSocket(enderecoHost: str, porta: int):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((enderecoHost, porta))
    sock.listen(1)

    while True:
        con, cliente = sock.accept()
        lerSocketThread = Thread(target=lerSocket, args=(con,))
        lerSocketThread.start()
        enviarSocketThread = Thread(target=enviarSocket, args=(con,))
        enviarSocketThread.start()


def ativarAlarme():
    global andares, alarmeAtivado
    pass


def sigint_handler(signal, frame):
    pass


if __name__ == "__main__":

    signal.signal(signal.SIGINT, sigint_handler)

    if(len(sys.argv) != 3):
        print(f"Use: {sys.argv[0]} <endereço ip> <porta>")
        print(f"Exemplo: {sys.argv[0]} 192.168.0.53 10055")
        os._exit(os.EX_OK)

    socketThread = Thread(target=initSocket, args=(str(sys.argv[1]),int(sys.argv[2]),))
    socketThread.isDaemon=True
    socketThread.start()

    interfaceThread = Thread(target= curses.wrapper, args=(pintarInterface,))
    interfaceThread.start()
    interfaceThread.join()

    os._exit(os.EX_OK)
