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

# drop down list-1: for date
# drop down list-2: for location
# drop down list-3: for sensor

"SELECT DISTINCT sensor_id FROM table"



app.layout = html.Div([
    html.H1('Stock Tickers'),
    html.H2('Sensors'),
    dcc.Dropdown(id='my-dropdown',  options=[{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}],value='None'),
    dcc.Graph(id='my-graph'),
    dcc.Dropdown(id='my-dropdown123',  options=[{'label': 'None', 'value': 'None'},{'label': 'Tesla', 'value': 'TSLA'},{'label': 'Apple', 'value': 'AAPL'},{'label': 'Coke', 'value': 'COKE'}],value='None'),
    dcc.Graph(id='my-graph123')
], className="container")



@app.callback(Output('my-graph', 'figure'),
              [Input('my-dropdown', 'value'), Input('my-dropdown123', 'value')])

def update_graph(selected_,selected_dropdown_value_0):
    print "000:", selected_dropdown_value, selected_dropdown_value_0
    dff = df[df['Stock'] == selected_dropdown_value]
    # print dff.Date
    return {
        'data': [{
            'x': s1,#dff.Date,
            'y': s2,#dff.Close,
            'line': {
                'width': 3,
                'shape': 'spline'
            }
        }],
        'layout': {
            'margin': {
                'l': 30,
                'r': 20,
                'b': 10,
                't': 20
            }
        }
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