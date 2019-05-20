load 'cosmos/gui/line_graph/line_graph_script.rb'

x = [0.0, 1.0, 2.0, 3.0, 4.0]
y1 = [10.5, 9.6, 7.7, 6.8, 7.4]
y2 = [10.2, 9.1, 7.3, 6.5, 7.8]

plot(x, y1)
plot(x, y1, 'y1', y2, 'y2')
