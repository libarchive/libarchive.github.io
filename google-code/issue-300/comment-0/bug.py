import zipfile
zf = zipfile.ZipFile('out.zip', 'w')
zf.writestr('file.txt', 'This is a test.\n')
zf.close()
