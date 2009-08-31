#!/usr/bin/python

# Program to process output of ctest with valgrind 
fh = open("nightlyMemCheck10.log","r")
fhw = open("valgrindSuppressionsIGSTK3_4_1c.supp","w")
igot = fh.readlines()
linenumber1 = 0
numlines = len(igot)
listSuppressions = []

#if no prior suppressions new????, put 0,
#otherwise start it off with that prior number
#numSuppressions = 0
numSuppressions = 1094

#process the ctest output, keying on valgrind output
for line in igot:
    where = line.find("<insert a suppression name here>")    
    if where  > -1:
        startLine= igot[linenumber1-1]
        #print "startLine",startLine
        where2 = startLine.find("{")
        #print "where2", where2
        if where2 > -1:
            linenumber2 = linenumber1
            for line2 in igot[linenumber1:numlines]:
                where3 = line2.find("}")
                linenumber2+= 1
                if where3 > -1:
                    label = "  new%d\n"%(numSuppressions+1)
                    newSuppression = []
                    newSuppression.append(startLine)
                    
                    #no need to add "<insert a suppression name here>"
                    for line2 in range(linenumber1+1, linenumber2):
                        newSuppression.append(igot[line2])
                        #print "line2",igot[line2]
                        
                    listIsFound = False
                    for listEntry in listSuppressions:
                        #print "listEntry[3:]", listEntry[3:]
                        #print "listIsFound", listIsFound
                        #print "newSuppression[2:]",newSuppression[2:]
                        if listEntry[3:]==newSuppression[2:]:
                            listIsFound= True

                    if (listIsFound == False):
                        numSuppressions+= 1
                        newSuppression.insert(1,label)
                        listSuppressions.append(newSuppression)
                        for lineSupp in newSuppression:
                            fhw.write(lineSupp)
                    print numSuppressions                
                    break
    linenumber1+= 1

fhw.close()
fh.close()
