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

s1 = ['1:0:0','2:0:9','3:1:2','4:2:3','1:1:1']
s2 = [1,2,3,4]

p1 = ['1:0:0','2:0:9','3:1:2','4:2:3']
p2 = [4,3,2,1]

def get_dates_entries():
    dates_dropdown = []
    query = 'SELECT table_name FROM information_schema.tables WHERE table_name LIKE "m%y%";'

    mycursor.execute(query)
    results = mycursor.fetchall()

    for each in results:
        # each = "m02y19"
        each = str(each[0]) # Hack: this returned a tuple, so did this processing
        disp_str = each[1:3] + ' / 19' + each[4:6]
        temp_dict = {}
        temp_dict['label'] = disp_str
        temp_dict['value'] = each
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

    # drop down list-2: for location
    dcc.Dropdown(id='location-dropdown',  options=location_dict),

    # drop down list-3: for sensor
    dcc.Dropdown(id='sensor-dropdown',  options=sensor_dict),
    
    dcc.Graph(id='sensor-readout'),

    dcc.Interval(id='interval-update', interval=1000, n_intervals=0)
    
    # dcc.Dropdown(id='location-dropdown',  options=[{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}],value='None'),
    # dcc.Graph(id='my-graph123')
], className="container")

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
    if select_date and select_location and select_sensor:
        query = None
        #TODO: in the below line make the date a variable
        if select_type == "date":
            query = "SELECT * FROM %s where location_id = %s and sensor_id = %s and date = %s;" % (select_date, select_location, select_sensor, 1) #TODO: read this from elsewhere
        else:
            # obtain the number of rows first
            query = "SELECT count(*) FROM %s where location_id = %s and sensor_id = %s;" % (select_date, select_location, select_sensor)
            mycursor.execute(query)
            COUNT = mycursor.fetchall()[0][0]
            print "COUNT:",COUNT
            query = "SELECT * FROM %s where location_id = %s and sensor_id = %s " % (select_date, select_location, select_sensor)
            if COUNT < ITEMS: query += "LIMIT %d offset %d;"%(ITEMS, COUNT)
            else: query += "LIMIT %d offset %d;"%(ITEMS, COUNT - ITEMS)
            

        print "query is:",query
        mycursor.execute(query)
        results = mycursor.fetchall()
        # print "results:",results
        mydb.commit()
        for each in results:
            x_axis.append(str(each[3])+":"+str(each[4]))
            y_axis.append(each[-1])

        units_query = "SELECT sensor_units FROM sensor WHERE sensor_id = %s;" % (select_sensor)
        mycursor.execute(units_query)
        y_units = mycursor.fetchall()[0][0]
        
    # print x_axis
    # print y_axis

    title = "This is the title"
    return {
        'data': [
        {
            'x': x_axis, 'y': y_axis,
            'line': {'width': 3, 'shape': 'spline'}
        }
        # ,{
        #     'x': p1, 'y': p2,
        #     'line': {'width': 3, 'shape': 'spline'}
        # }
        ],
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