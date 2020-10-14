rem convert test.zip to test.hex
del test.hex
CertUtil -encodehex test.zip test.hex 4
