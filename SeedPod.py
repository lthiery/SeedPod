import spi
import time

spi.initialize()

def getData():
	dataIn = spi.transfer((3,0,0,0,0,0,0,0))
	#print dataIn
	for index,item in enumerate(dataIn):
		try:
			if item==3 and dataIn[index+3]==(dataIn[index+1]+dataIn[index+2]&0xFF):
				out = round((dataIn[index+1]<<8 | dataIn[index+2]) / float(0b111111111111)*5, 2)
				print out, "V"
				break
		except:
			pass

while True:
	getData()
