#!/usr/bin/python

import sys

fileName = ''

if (len(sys.argv) == 2):
	fileName = sys.argv[1]
else:
	print 'No filename provided - exiting'
	sys.exit()
	
text = open(fileName)
newText = open(fileName + '.new', "w")
for line in text.readlines():
	if not '_double' in line: 
		newText.write(line)
		continue
	retVal = line.split("=")[0].strip()
	funcName = line.split("=")[1].split("(")[0].strip()
	if funcName != '_doubleneg':
		funcVars = line.split("=")[1].split("(")[1].split(",", 5)
		funcVars[4] = funcVars[4].split(')')[0]
		newLine = '%s(&%s, &%s, &%s, %s, %s, %s);\n' % (funcName.strip(), funcVars[0].strip(), funcVars[1].strip(), retVal.strip(), funcVars[2].strip(), funcVars[3].strip(), funcVars[4].strip())
	else:
		funcVars = line.split("=")[1].split("(")[1].split(",", 4)
		funcVars[3] = funcVars[3].split(')')[0]
		newLine = '%s(&%s, &%s, %s, %s, %s);\n' % (funcName.strip(), funcVars[0].strip(), retVal.strip(), funcVars[1].strip(), funcVars[2].strip(), funcVars[3].strip())
	
	newText.write(newLine)
text.close()
