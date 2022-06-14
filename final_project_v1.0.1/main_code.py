# ━━━━━━━━━━━━━━━━━━━━━ Library ━━━━━━━━━━━━━━━━━━━━━
import time,pyrebase,RPi.GPIO as GPIO
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

# ━━━━━━━━━━━━━━━━━━━━━ Variable definition ━━━━━━━━━━━━━━━━━━━━━
company,path,machineID,errorcode = "",db,"machineID","200" # Machine ID definitions
hourNow,tempHour= datetime.now().hour, datetime.now().hour # Now time
TRIG_Pin,ECHO_Pin=6,5 # Pin definition

# ━━━━━━━━━━━━━━━━━━━━━ Distance function ━━━━━━━━━━━━━━━━━━━━━
def distance(): # Distance
    #global path
    GPIO.output(TRIG_Pin, False)
    time.sleep(2)
    GPIO.output(TRIG_Pin, True)
    time.sleep(0.00001)
    GPIO.output(TRIG_Pin, False)
    while GPIO.input(ECHO_Pin)==0:
        pulse_start = time.time()
    while GPIO.input(ECHO_Pin)==1:
        pulse_end = time.time()
    pulse_duration = pulse_end-pulse_start
    distance = pulse_duration*17150
    distance = round(distance,2)
    return distance

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

# ━━━━━━━━━━━━━━━━━━━━━ Loop function ━━━━━━━━━━━━━━━━━━━━━
def loop(): # Loop
    global db,company,path,machineID,errorcode
    global hourNow,tempHour
    global TRIG_Pin,ECHO_Pin
    hourNow,tempHour= datetime.now().hour, datetime.now().hour # Now time
    print(hourNow)

# ━━━━━━━━━━━━━━━━━━━━━ Main Code ━━━━━━━━━━━━━━━━━━━━━
if __name__ == "__main__": # Main funtion
    setup() # Setup worked
    while True: # Loop started
        loop()