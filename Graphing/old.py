import dash
from dash.dependencies import Output, Event
impor tdash_core_components as html
import plotly
import random
import plotly.graph_objs as go
from collections import deque

X = deque(maxlen=20)
Y = deque(maxlen=20)

X.append(1)
Y.append(1)

app = dash.Dash(__name__)
app.layout = html.Div([dcc.Graph(id='live-graph', animate=True)
                       dcc.Interval(id='graph-update', interval=1000)])

@app.callback(Output('live-graph', 'figure'),events = [Event('graph-update')])