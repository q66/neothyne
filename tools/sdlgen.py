#!/usr/bin/env python

import sys, os
from proto import *

def procName(function):
    return function.name.replace('_', '').upper()

def genTypedef(source, function):
    source.write('typedef %s (*PFN%sPROC)' % (function.type, procName(function)))
    printFormals(sys.stdout, function, False)
    source.write(';\n')

def genStatic(source, function, largest):
    name = procName(function)
    fillProc = largest - len(name)
    fillName = largest - len(function.name)
    source.write('static PFN%sPROC %s %s_ %s= nullptr;\n'
        % (name,
           ''.rjust(fillProc),
           function.name,
           ''.rjust(fillName))
    )

def genLoad(source, function, largest):
    fillName = largest - len(function.name)
    source.write('%s_ %s= (PFN%sPROC)gSDL("%s");\n'
        % (function.name,
           ''.rjust(fillName),
           procName(function),
           function.name))

def main(argv):
    protos = 'tools/sdlprotos'
    functionList = readPrototypes(protos)

    for function in functionList:
        genTypedef(sys.stdout, function)

    sys.stdout.write('\n')

    largest = max(len(x.name) for x in functionList)
    for function in functionList:
        genStatic(sys.stdout, function, largest)

    for function in functionList:
        genLoad(sys.stdout, function, largest)

if __name__ == "__main__":
    main(sys.argv)
