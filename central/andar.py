
from inout import InOut

class Andar:
    def __init__(self, nome):
        self.nome = nome
        self.inputs = []
        self.outputs = []
        self.temperatura = 0
        self.humidade = 0
        self.contagem = InOut(None, None, 0, 0)
        self.fumaca = InOut(None, None, 0, 0)

    def addInput(self, input: InOut):
        self.inputs.append(input)

    def addOutput(self, output: InOut):
        self.outputs.append(output)

    def setTemperatura(self, temperatura):
        self.temperatura = temperatura

    def setHumidade(self, humidade):
        self.humidade = humidade

    def setContagem(self, contagem):
        self.contagem = contagem

    def setFumaca(self, fumaca):
        self.fumaca = fumaca

    def getNome(self):
        return self.nome

    def getInputs(self):
        return self.inputs

    def getOutputs(self):
        return self.outputs

    def getTemperatura(self):
        return self.temperatura

    def getHumidade(self):
        return self.humidade

    def getContagem(self):
        return self.contagem

    def getFumaca(self):
        return self.fumaca

