rem convert file.hex to file.zip

del file.zip
CertUtil -decodehex file.hex file.zip