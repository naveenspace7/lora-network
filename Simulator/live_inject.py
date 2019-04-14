import mysql.connector
import time
import datetime

# print "Connecting to database " + db + " at " + host + " with credentials " + user + " " + pwd
mydb = mysql.connector.connect(host="127.0.0.1", user="root", passwd="samaritan3", database='lora_test')
mycursor = mydb.cursor()

temperature_run_1 = [12, 12, 12, 13, 13, 10, 10, 10, 13, 15, 14, 14, 14, 18, 14, 15, 15, 14, 14, 14, 13, 15]
temperature_run_2 = [12, 14, 18, 17, 18, 14, 14, 15, 15, 14, 14, 14, 13, 13, 13, 15, 15, 15, 16, 16, 14, 16]

humidity_run_1    = [89, 89, 89, 85, 86, 86, 90, 90, 90, 95, 95, 94, 98, 98, 97, 97, 92, 90, 91, 91, 97, 93]
humidity_run_2    = [86, 89, 89, 86, 90, 92, 93, 97, 96, 96, 97, 94, 94, 95, 93, 94, 94, 92, 93, 94, 93, 89]

co2_run_1 = [103, 103, 103, 105, 105, 107, 107, 108, 105, 103, 100, 134, 134, 134, 134, 134, 134, 134, 134, 134, 120, 132, 131]
co2_run_2 = [120, 122, 122, 121, 120, 120, 123, 123, 123, 124, 124, 130, 131, 131, 132, 135, 134, 134, 134, 135, 125, 129, 132]

i = 0

strg = []

while True:

    now = datetime.datetime.now()

    date = now.day
    hr = now.hour
    min = now.minute

    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 5, 23, hr, min, temperature_run_1[i])) # temp
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 5, 5, hr, min, humidity_run_1[i])) # hum
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 5, 45, hr, min, co2_run_1[i])) # gas

    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 43, 23, hr, min, temperature_run_2[i]))
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 43, 5, hr, min, humidity_run_2[i]))
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 43, 46, hr, min, co2_run_2[i]))

    for sql_insert_query in strg:
        cursor = mydb.cursor()
        result  = cursor.execute(sql_insert_query)
        mydb.commit()
    
    print "Injected a set"

    if i > 20: i = 0
    else: i+=1

    time.sleep(60)