from fprime_ac.parsers import XmlComponentParser
import os


## Manual RecvBuffComponent
## has the following implicit ports:
#
#  CmdDisp
#  CmdRegistration
#  CmdStatus
#
#  ParamGet
#  ParamSet
#
#  Telemetry
#
#  TimeGet
#
#  LogEvent
#  LogTextEvent
def ComponentParser_manualxml_test():

	implicit_port_roles = ['Cmd',
						   'CmdResponse',
						   'CmdRegistration',
						   'ParamGet',
						   'ParamSet']

	xmlfile = os.path.join(os.getcwd(), "man_RecvBuffComponentAi.xml")

	parsed_xml = XmlComponentParser.XmlComponentParser(xmlfile)



	for required_port in implicit_port_roles:
		found = False
		for p in parsed_xml.get_ports():
			if required_port == p.get_role():
				found = True
		if found:
			assert True
		else:
			print("%s port is required but not present." % required_port)
			assert False

## If xml is missing a port XmlComponentParser should exit
## Missing roles:
#
#  ParamGet
#  Cmd
def ComponentParser_missingport_test():

	xmlfile = os.path.join(os.getcwd(), "missing_port_SendBuffComponentAi.xml")

	XmlComponentParser.XmlComponentParser(xmlfile)

	assert True
