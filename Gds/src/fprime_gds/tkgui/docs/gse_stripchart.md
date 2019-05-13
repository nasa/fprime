# StripChart Docs

This markdown document covers the usage and class structure of
the GSE stripchart panel.


## Usage
-----
#### Adding stripchart or histogram
- Click the 'Add' button for the respective chart.
- Add channels to the graph

#### Changing Visual Render Rate
- Click 'Settings' button

#### Changing Plot Dimensions
- Click 'Settings' button

#### Changing Plot Labels
- Click 'Label Settings'

#### Configuring Grid Geometry
- Change stripcharts, max_columns setting in gse.ini

#### Adding Max, Min limit lines
- 'channel_max' and 'channel_min' properties can be configured in gse.ini
- Add channel name to BOTH properties to show limit lines.
- The current gse.ini has an example for 'Sensor1'

#### Viewing amplitude frequency spectrum
- Click 'Spectrum while running a stripchart.'
- Note: Returning to time domain is bugged. Create a new graph with the same channel.

#### Saving Plots
- Press 'Save All' button and select folder to save into

#### Limitations/Know Issues
- Plot creation & deletion is hard on Tkinter Grid Manager. Maybe better ways to handle this.
- Cannot return to time domain after toggling spectral display.
- Creating plots creates exit issues...even if plots are explicitly destroyed.

## Class Structure
----
The involved classes are:
- StripChartListener
- StripChartPanel
- AxesFrame and it's subclasses: StripFrame, HistFrame
- StripMode

#### StripChartListener
StripChartListener maintains a list of StripChartPanel observers. StripChartListener is given data from
EventListener and ChannelListener. Data is passed to the StripPanels during an update call.


#### StripChartPanel

In the front, StripChartPanel controls general settings, plot saving, and the creation of new AxesFrames.
General settings include:
- Plot height (Width scales with window)
- Visual refresh rate

Default settings can be configured in gse.ini.
The number of columns in the grid can also be specified in the gse.ini file.


The 'Save Plots' allows the user to save all plots to a selected folder.
Add StripChart and Add Histogram buttons create the respective AxesFrame subclasses. AxesFrames are inserted into a Pmw ScrolledFrame widget using the grid geometry manager.


In the back StripChartpanel recieves updates from StripChartListener, periodically refreshes it's AxesFrames, and handles destruction of the panel.

Updates are send to StripChartPanel instances by the StripChartListener. Updates may contain new Channel data and Event data. StripChartPanel sends data to the appropriate AxesFrame. It filters out unneeded data.

#### AxesFrame and subclasses
AxesFrame has two subclasses: StripFrame and HistFrame.

An AxesFrames recieves data from StripChartPanel and adds it to their model. Data is only rendered when StripChartPanel calls refresh().

#### StripFrame
After channels are added channel data is recieved via calls to update_telem(). Incoming data is streamed into a fixed sized circular buffer. This allows data to indefinetly stream into a StripFrame.

During refresh StripFrame loads the circular buffer contents into a matplotlib line object, handles scaling, then draws the canvas.

#### HistFrame
Histograms watches events, channels, or severities and keeps a count of how frequently they occur in relation to other watched items.


#### StripMode
StripMode encapsulates the stripchart window scaling logic. Different calls to StripMode set and handle scaling
parameters and operations.
