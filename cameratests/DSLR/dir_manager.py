
import os 
from datetime import datetime



#x = int("deadbeef", 16)

date = datetime.today().strftime('%Y-%m-%d')
datapath = '/home/iris/iris/data/'+date+'/'

def filepathinit():
    if os.path.isdir(datapath):
        print("Dir Enabled") 
        files = os.listdir(datapath)
        image_count = len(files)
        print('Pictures saved from: '+str(image_count))
    else:
        print("Creating Dir")
        os.mkdir(datapath) 
        filepathinit()

filepathinit()




