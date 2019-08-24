# HDF5 Writer
## Overview
The HDF5 Writer, as the name suggests, writes various types of events into an hdf5 file. After discussing over the different ways in which data can be stored in an hdf5 file, the current implementation creates a group for each event and then writes the each collection as an individual dataset inside that group. Thus for instance one can expect to see the following structure in an hdf5 file:  
1/EventInfo  
1/ExampleMC  
2/ExampleWithString  
2/ExampleWithVectorMembers  
and so on. Clearly this way of writing data is not definitive and we hope to experiment with some other styles in the future. 
## Implementation
The current implementation first requires the auto-generation of the hdf5 code for each collection described in the yaml files. For generating an hdf5 files with directories like structure run hdf5_class_generator_directories.py and for generating hdf5 files with extendible dataset with RANK=2 use hdf5_class_genertor.py in the /python directory.  
Next, include/podio/ and src/ directories contain the scripts for the classes H5Collections and HDF5Writer. H5Collections is an abstract class and each collection in the yaml file is child of this class. Thus H5EventInfoCollection is derived from H5Collection, where H5EventInfoCollection has all the necessary code to write EventInfoCollection to an hdf5 file. The HDF5Writer acts as an interface in that it registers each hdf5 collection and calls the specific writing code for each collection.   
## Testing
To run the code one only needs to build the project and run the executable ./write_hdf5 in the build/tests directory. This generates the file dummy.h5 which can be viewed through h5dump or any other software of choice. 

