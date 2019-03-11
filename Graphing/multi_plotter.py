#!/usr/bin/python

import dash
from dash.dependencies import Input, Output
import dash_core_components as dcc
import dash_html_components as html
import flask
import pandas as pd
import time
import os, datetime

import mysql.connector

from lxml import etree

def parseXML(xmlFile):
    """
    Parse the xml
    """
    user = ''; ip_addr = ''; pwd=''; db=''
    with open(xmlFile) as fobj:
        xml = fobj.read()

    root = etree.fromstring(xml)

    for appt in root.getchildren():
        for elem in appt.getchildren():
            if not elem.text:
                text = "None"
            else:
                text = elem.text

            if elem.tag == "ip_addr":
                ip_addr = text
            elif elem.tag == "user":
                user = text
            elif elem.tag == "pwd":
                pwd = text
            elif elem.tag == "db":
                db = text

            # print(elem.tag + " => " + text)

    return user, ip_addr, pwd, db

user, host, pwd, db = parseXML("credentials.xml")

server = flask.Flask('app')
server.secret_key = os.environ.get('secret_key', 'secret')
app = dash.Dash('app', server=server)
app.scripts.config.serve_locally = False

print "Connecting to database " + db + " at " + host + " with credentials " + user + " " + pwd
mydb = mysql.connector.connect(host=host, user=user, passwd=pwd, database=db)
mycursor = mydb.cursor()

location_mapping = {}

def get_dates_entries():
    dates_dropdown = []
    # query = 'SELECT table_name FROM information_schema.tables WHERE table_name LIKE "m%y%";'

    query = "select distinct date from m03y2019;"
    # print "qw:",query

    mycursor.execute(query)
    results = mycursor.fetchall()
    # print "res:",results

    for each in results:
        # each = "m02y19"
        each = str(each[0]) # Hack: this returned a tuple, so did this processing
        disp_str = each
        temp_dict = {}
        temp_dict['label'] = disp_str + " - 03 - 2019"
        temp_dict['value'] = "m03y2019"
        dates_dropdown.append(temp_dict)
    return dates_dropdown

def get_location_entries(date_str = None):
    location_dropdown = []
    if date_str != None: return
    date_str = "m03y2019"
    query = "select distinct location_name, location.location_id from location inner join " + date_str + " on location.location_id = m03y2019.location_id;"
    mycursor.execute(query)
    results = mycursor.fetchall()
    for each in results:
        temp_dict = {}
        temp_dict['label'] = str(each[0])
        temp_dict['value'] = str(each[1])
        location_dropdown.append(temp_dict)
        # location_mapping.append(temp_dict)
        location_mapping[each[1]] = str(each[0])
    # print location_mapping
    return location_dropdown

def get_sensor_entries(date_str = None):
    sensor_dropdown = []
    if date_str != None: return
    date_str = "m03y2019"
    query = "select distinct sensor_name, sensor.sensor_id from sensor inner join " + date_str + " on sensor.sensor_id = m03y2019.sensor_id;"
    mycursor.execute(query)
    results = mycursor.fetchall()
    for each in results:
        temp_dict = {}
        temp_dict['label'] = str(each[0])
        temp_dict['value'] = str(each[1])
        sensor_dropdown.append(temp_dict)
    return sensor_dropdown

# date_dict = [{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}]#get_dates_entries()
# location_dict = [{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}]#get_dates_entries()
# sensor_dict = [{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}]#get_dates_entries()
date_dict = get_dates_entries()
location_dict = get_location_entries()
sensor_dict = get_sensor_entries()

app.layout = html.Div([
    html.H1('Sensors'),
    # dcc.Dropdown(id='date-dropdown',  options=[{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}],value='None'),
    
    dcc.RadioItems(id='type-selection', options=[{'label':'Date', 'value':'date'}, {'label':'Real-time', 'value':'realtime'}], value='realtime',labelStyle={'display':'inline-block'}),

    # drop down list-1: for date
    dcc.Dropdown(id='date-dropdown',  options=date_dict),
    html.Div('br'),

    # drop down list-3: for sensor
    dcc.Dropdown(id='sensor-dropdown',  options=sensor_dict),

    # drop down list-2: for location
    dcc.Dropdown(id='location-dropdown',  options=location_dict, multi=True),

    
    dcc.Graph(id='sensor-readout'),

    dcc.Interval(id='interval-update', interval=5000, n_intervals=5)
    
], className="container")

# TODO: add the room name here and also the title of the graph
def obtain(locations,x,y):
    # print "in obtain:", location_mapping[int(locations)]
    return {'x': x[:-1], 'y': y[:-1],'line': {'width': 3, 'shape': 'spline'}, 'name':location_mapping[int(locations)]}

@app.callback(Output('sensor-readout', 'figure'),
              [Input('date-dropdown', 'value'), 
               Input('type-selection', 'value'),
               Input('location-dropdown', 'value'), 
               Input('sensor-dropdown', 'value'),
               Input('interval-update', 'n_intervals')])
def update_graph(select_date, select_type, select_location, select_sensor, n):
    print "Dropbox selection:", select_date, select_location, select_sensor, select_type, n
    x_axis = []
    y_axis = []
    y_units = None; x_units = "Time"
    ITEMS = 20
    now = datetime.datetime.now().day
    only_mins = []
    mins_fill = False
    if select_date and select_location and select_sensor:
        for each_location in select_location:
            print "inloop"
            query = None
            #TODO: in the below line make the date a variable
            if select_type == "date":
                query = "SELECT * FROM %s where location_id = %s and sensor_id = %s and date = %s;" % (select_date, each_location, select_sensor, 4) #TODO: read this from elsewhere
            else:
                # obtain the number of rows first
                query = "SELECT count(*) FROM %s where location_id = %s and sensor_id = %s;" % (select_date, each_location, select_sensor)
                # print "sub-query is:",query
                mycursor.execute(query)
                COUNT = int(mycursor.fetchall()[0][0])
                # print "COUNT:",COUNT
                query = "SELECT * FROM %s where location_id = %s and sensor_id = %s " % (select_date, each_location, select_sensor)
                if COUNT < ITEMS: query += "LIMIT %d offset %d;"%(ITEMS, 0)
                else: query += "LIMIT %d offset %d;"%(ITEMS, COUNT - ITEMS)

            print "query is:",query
            mycursor.execute(query)
            results = mycursor.fetchall()
            print "results:",results
            mydb.commit()
            x_temp = []; y_temp = []
            values_dict = {}
            for each in results:
                x_temp.append(str(each[3])+":"+str(each[4]))
                y_temp.append(each[-1])
                # values_dict[str(each[3])+":"+str(each[4])] = each[-1]
            
            # mins_fill = True

            units_query = "SELECT sensor_units FROM sensor WHERE sensor_id = %s;" % (select_sensor)
            mycursor.execute(units_query)
            y_units = mycursor.fetchall()[0][0]

            x_axis.append(x_temp); y_axis.append(y_temp)


    data = []

    for s in range(0,len(x_axis)):
        data.append(obtain(select_location[s], x_axis[s], y_axis[s]))


        
    # print x_axis
    # print y_axis

    title = "This is the title"
    return {
        'data': data,
        # [
        # {
        #     'x': s1, 'y': s2,
        #     'line': {'width': 3, 'shape': 'spline'}
        # }
        # ,{
        #     'x': p1, 'y': p2,
        #     'line': {'width': 3, 'shape': 'spline'}
        # }
        # ],
        'layout': 
        {
            'title': title,
            'margin': {
                'l': 30,
                'r': 20,
                'b': 30,
                't': 20
            },
            'xaxis': {'title': x_units},
            'yaxis': {'title': y_units}
        },
    }


if __name__ == '__main__':
    app.run_server()