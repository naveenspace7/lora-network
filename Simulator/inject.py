import mysql.connector

# print "Connecting to database " + db + " at " + host + " with credentials " + user + " " + pwd
mydb = mysql.connector.connect(host="127.0.0.1", user="root", passwd="samaritan3", database='lora_test')
mycursor = mydb.cursor()

file = open("values_injection.csv", 'r') 
f = file.readline()
while True:
    f = file.readline().strip()
    if f == '': break
    list = f.split(',')
    print list

    date = int(list[0].split('/')[0])
    min = int(list[1].split(':')[1])
    hr = int(list[1].split(':')[0])
    
    sensor_location_values = list[4].split()

    val = sensor_location_values[0].split('/')

    strg = []
    

    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 5, 23, hr, min, int(val[0])))
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 5, 5, hr, min, int(val[1])))
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 5, 45, hr, min, int(val[2])))

    # print strg
    val = sensor_location_values[1].split('/')
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 43, 23, hr, min, int(val[0])))
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 43, 5, hr, min, int(val[1])))
    strg.append("INSERT INTO m03y2019(date, location_id, sensor_id, hour, min, value) VALUES(%d, %d, %d, %d, %d, %d);" % (date, 43, 46, hr, min, int(val[2])))

    for sql_insert_query in strg:
        cursor = mydb.cursor()
        result  = cursor.execute(sql_insert_query)
        mydb.commit()

    # CREATE TABLE m03y2019 (date int(3), location_id varchar(20), sensor_id varchar(20), hour varchar(20), min varchar(20), value varchar(20));
