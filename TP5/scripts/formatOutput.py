import os
import sys
import fileinput


#concat all files generated by Hadoop

os.system("cat output/part* >> output/OutFile.txt")



files='[\n\t'

for file in os.listdir("./input"):
    files=files+file+'\n\t'

files=files+']\n{\n\t'

fileToSearch = 'output/OutFile.txt'
fileout = 'results/IndexInverse.txt'

textToSearch='['
textToReplace='[\n\t('
textToSearch2=']'
textToReplace2=')\n]'
textToSearch3=','
textToReplace3=')\n\t('
textToSearch4='( '
textToReplace4='('

f = open(fileToSearch,'r')
filedata = f.read()
f.close()

newdata = filedata.replace(textToSearch,textToReplace)
newdata = newdata.replace(textToSearch2,textToReplace2)
newdata = newdata.replace(textToSearch3,textToReplace3)
newdata = newdata.replace(textToSearch4,textToReplace4)
newdata=files+newdata+"\n}"


f = open(fileout,'w')
f.write(newdata)
f.close()
