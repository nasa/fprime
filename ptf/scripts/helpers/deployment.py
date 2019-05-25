import os
import xml.sax
import scripts.helpers.xml_readers.env_xml_reader
import scripts.helpers.target_helper_factory
import scripts.helpers.master_helper_factory
import scripts.helpers.simulation_helper_factory
import scripts.helpers.gds_helper_factory


class Deployment:
    def __init__(self):

        self.master_helper = None
        self.simulation_helper = None
        self.gds_helper = None
        self.target_helper = None
        
        # get deployment type
        deployment = os.environ["DEPLOYMENT"] 

        # read deployment XML file
        deployment_file = msl_root + "/ptf/configurations/deployment/" + options.deployment + ".xml"
        if (os.path.isfile(deployment_file) == False):
            print("Deployment configuration file " + target_file + " does not exist")
            sys.exit(-1)        
            
        deployment_cfg_handler = scripts.helpers.xml_readers.env_xml_reader.EnvXMLHandler()   
        xml.sax.parse(deployment_file,deployment_cfg_handler)
        
        # retrieve dictionary
        deployment_component_dictionary = deployment_cfg_handler.getDict()
        
        for component in list(deployment_component_dictionary.keys()):
            # parse xml files into environment
            component_file = "/ptf/configurations/" + component + "/" + deployment_component_dictionary[component]
  
            if (os.path.isfile(component_file) == False):
                print("Deployment component configuration file " + target_file + " does not exist")
                sys.exit(-1)
                        
            deployment_component_cfg_handler = scripts.helpers.xml_readers.env_xml_reader.EnvXMLHandler()
            xml.sax.parse(component_file,deployment_component_cfg_handler)
            deployment_component_cfg_handler.toEnv()
            deployment_component_cfg_handler.clear()
            
            # check for component type
            
            if component == "master":
                self.master_helper = scripts.helpers.master_helper_factory.MasterHelperFactory().getMasterHelper()
                self.master_helper.start_master()
            elif component == "simulation":
                self.simulation_helper = scripts.helpers.simulation_helper_factory.SimulationHelperFactory().getSimulationHelper()
                self.simulation_helper.start_simulation()
            elif component == "gds":
                self.gds_helper = scripts.helpers.gds_helper_factory.GdsHelperFactory().getGdsHelper()
                self.gds_helper.start_gds()
            elif component == "target":
                self.target_helper = scripts.helpers.target_helper_factory.TargetHelperFactory().getTargetHelper()
                self.target_helper.start_target()
            else:
                print("Unknown component type " + component + " in deployment " + deployment + "\n")
                assert("Unknown component")
                
    def exit(self):

        if self.target_helper:
            self.target_helper.exit()
            self.target_helper.wait()
            
        if self.gds_helper:
            self.gds_helper.exit()
            self.gds_helper.wait()

        if self.simulation_helper:
            self.simulation_helper.exit()
            self.target_helper.wait()

        if self.master_helper:
            self.master_helper.exit()
            self.master_helper.wait()
                
    def send_packet(self, filename):
        self.gds_helper.send_packet(filename)
        
    def run(self,ticks):
        self.master_helper.run(ticks)
        
    def run_command(self, command):
        self.target_helper.run_cmd(command)
        
    def load_fsw(self, file=None):
        self.target_helper.load_fsw(file)
                
            
