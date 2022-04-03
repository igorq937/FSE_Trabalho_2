
from inout import InOut

class Andar:
    def __init__(self, nome):
        self.nome = nome
        self.inputs = []
        self.outputs = []
        self.dht = None

    def addInput(self, input: InOut):
        self.inputs.append(input)

    def addOutput(self, output: InOut):
        self.outputs.append(output)

    def setDht(self, dht: InOut):
        self.dht = dht

    def getNome(self):
        return self.nome

    def getInputs(self):
        return self.inputs

    def getOutputs(self):
        return self.outputs

    def getDht(self):
        return self.dht
