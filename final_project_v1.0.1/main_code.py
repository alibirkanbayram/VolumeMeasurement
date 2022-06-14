import time,RPi.GPIO as GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

# ━━━━━━━━━━━━━━━━━━━━━ Firebase definition ━━━━━━━━━━━━━━━━━━━━━
config = {
  "apiKey": "api_key",
  "authDomain": "auth_domain",
  "databaseURL": "database_url",
  "storageBucket": "storage"
}
firebase = pyrebase.initialize_app(config)
db = firebase.database()

company,path,machineID,errorcode = "",db,"7539",200 # Machine ID definitions
hourNow,tempHour= datetime.now().hour, datetime.now().hour # Now time
TRIG_Pin,ECHO_Pin=6,5 # Pin definition

# ━━━━━━━━━━━━━━━━━━━━━ Setup function ━━━━━━━━━━━━━━━━━━━━━
def setup(): #Setup
    global db,company,path,machineID,errorcode
    global hourNow,tempHour
    global TRIG_Pin,ECHO_Pin
    company=path.child("SETUP").child(machineID).get().val()
    print(company)
    path=db.child("COMPANY").child(company).child(machineID)
    path.child("INFO").update({"ERROR_CODE": errorcode,})
    GPIO.setup(ECHO_Pin, GPIO.IN)
    GPIO.setup(TRIG_Pin, GPIO.OUT)
    print("Setup ok")

def loop(): # Loop function
    global db,company,path,machineID,errorcode
    global hourNow,tempHour
    global TRIG_Pin,ECHO_Pin
    hourNow,tempHour= datetime.now().hour, datetime.now().hour # Now time
    print(hourNow)

if __name__ == "__main__": # Main funtion
    setup() # Setup worked
    while True: # Loop started
        loop()