import dash
from dash.dependencies import Input, Output
import dash_core_components as dcc
import dash_html_components as html

import flask
import pandas as pd
import time
import os

server = flask.Flask('app')
server.secret_key = os.environ.get('secret_key', 'secret')

df = pd.read_csv('https://raw.githubusercontent.com/plotly/datasets/master/hello-world-stock.csv')

app = dash.Dash('app', server=server)

app.scripts.config.serve_locally = False
# dcc._js_dist[0]['external_url'] = 'https://cdn.plot.ly/plotly-basic-latest.min.js'

s1 = ['1:0:0','2:0:9','3:1:2','4:2:3']
s2 = [1,2,3,4]

p1 = ['1:0:0','2:0:9','3:1:2','4:2:3']
p2 = [4,3,2,1]


# drop down list-3: for sensor


def get_dates_entries():
    dates_dropdown = []
    query = 'SELECT table_name FROM information_schema.tables WHERE table_name LIKE "m%y%";'

    # mycursor.execute(query)
    # results = mycursor.fetchall()
    results = []

    for each in results:
        # each = "m02y19"
        disp_str = each[1:3] + ' / 20' + each[4:6]
        temp_dict = {}
        temp_dict['label'] = disp_str
        temp_dict['value'] = each
        dates_dropdown.append(temp_dict)
    return dates_dropdown

def get_location_entries(date_str = None):
    dates_dropdown = []
    if date_str != None: return
    query = 'SELECT DISTINCT location_id FROM ' + date_str + ';'
    # mycursor.execute(query)
    # results = mycursor.fetchall()
    results = []
    for each in results:
        # disp_str = each[1:3] + ' / 20' + each[4:6] # TODO: get the location names from location IDs
        temp_dict = {}
        temp_dict['label'] = each
        temp_dict['value'] = each
        dates_dropdown.append(temp_dict)
    return dates_dropdown

def get_sensor_entries(date_str = None):
    dates_dropdown = []
    if date_str != None: return
    query = 'SELECT DISTINCT sensor_id FROM ' + date_str + ';'
    # mycursor.execute(query)
    # results = mycursor.fetchall()
    results = []
    for each in results:
        # disp_str = each[1:3] + ' / 20' + each[4:6] # TODO: get the sensor names from sensor IDs
        temp_dict = {}
        temp_dict['label'] = each
        temp_dict['value'] = each
        dates_dropdown.append(temp_dict)
    return dates_dropdown

"SELECT DISTINCT sensor_id FROM table"
date_dict = [{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}]#get_dates_entries()
location_dict = [{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}]#get_location_entries()
sensor_dict = [{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}]#get_sensor_entries()







app.layout = html.Div([
    html.H1('Stock Tickers'),
    html.H2('Sensors'),
    # dcc.Dropdown(id='date-dropdown',  options=[{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}],value='None'),
    
    # drop down list-1: for date
    dcc.Dropdown(id='date-dropdown',  options=date_dict),

    # drop down list-2: for location
    dcc.Dropdown(id='location-dropdown',  options=location_dict),

    # drop down list-3: for sensor
    dcc.Dropdown(id='sensor-dropdown',  options=sensor_dict),
    
    dcc.Graph(id='sensor-readout'),
    
    # dcc.Dropdown(id='location-dropdown',  options=[{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}],value='None'),
    # dcc.Graph(id='my-graph123')
], className="container")



@app.callback(Output('sensor-readout', 'figure'),
              [Input('date-dropdown', 'value'), 
               Input('location-dropdown', 'value'), 
               Input('sensor-dropdown', 'value')])

def update_graph(select_date, select_location, select_sensor):
    print "000:", select_date, select_location, select_sensor


    # dff = df[df['Stock'] == selected_dropdown_value]
    # print dff.Date

    title = "This is the title"
    return {
        'data': [{
            'x': s1,
            'y': s2,
            'line': {
                'width': 3,
                'shape': 'spline'
            }
        },
        {
            'x': p1,
            'y': p2,
            'line': {
                'width': 3,
                'shape': 'spline'
            }
        }
        ],
        'layout': {
            'title': title,
            'margin': {
                'l': 30,
                'r': 20,
                'b': 30,
                't': 20
            },
            'xaxis': {'title': "Time"},
            'yaxis': {'title': "Value"}
        },
    }

# @app.callback(Output('my-graph123', 'figure'),
#               [Input('my-dropdown123', 'value')])

# def update_graph(selected_dropdown_value):
#     print "123:",selected_dropdown_value
#     dff = df[df['Stock'] == selected_dropdown_value]
#     # print dff.Date
#     return {
#         'data': [{
#             'x': p1,#dff.Date,
#             'y': p2,#dff.Close,
#             'line': {
#                 'width': 3,
#                 'shape': 'spline'
#             }
#         }],
#         'layout': {
#             'margin': {
#                 'l': 30,
#                 'r': 20,
#                 'b': 10,
#                 't': 20
#             }
#         }
#     }

if __name__ == '__main__':
    app.run_server()