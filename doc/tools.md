# Tools and utilities

## Displaying the content of files: `podio-dump`
`podio-dump` is a command-line tool for inspecting the contents of PODIO
files. It prints information about available data models, frame categories, and
collections stored in the file, along with their details. The tool is useful for
quickly exploring and verifying PODIO files from the terminal.

You can fine-tune the dump:
- Use `-d` to print detailed, full contents of each entry, rather than just collection info.
- Use `-c` to select a specific frame category (such as "events" or "metadata", if available).
- Use `-e` to choose which entry or range of entries to print.
To see all available options and usage details, run with `-h` or `--help`.

Example usage:

```
podio-dump file.root
```

Sample output (EDM4hep file):
``` text
input file: functional_producer.root
            (written with podio version: 1.4.0)

datamodel model definitions stored in this file:
 - edm4hep (0.99.99)

Frame categories in this file:
Name                    Entries
----------------------  -------
events                  10
configuration_metadata  1
################################### events: 0 ####################################
Collections:
Name         ValueType            Size  ID
-----------  -------------------  ----  --------
MCParticles  edm4hep::MCParticle  2     a1cba250

Parameters:
Name  Type  Elements
----  ----  --------
```

Note that for ROOT files it is also possible to display the data using tools
from ROOT (like `rootbrowse`), however losing all the functionality provided by
PODIO.

## Merging files: `podio-merge-files`
`podio-merge-files` is a command-line tool for merging multiple PODIO files
(both TTree and RNTuple formats) into a single output file. It preserves all
data categories and offers flexible handling of metadata:
- `--metadata=first` (default): include only metadata from the first input file.
- `--metadata=all`: include metadata from all input files.
- `--metadata=none`: omit metadata in the merged file.
In addition, a parameter with all the input files will be added to the output file.

You must specify an output file with `--output-file` and a list of files to
merge. The tool checks for duplicate files in the input list and raises an error
if any are found. To see all available options and details, run with `-h`.

Note that for ROOT files it is also possible to use the `hadd` tool from ROOT,
although it may duplicate some information like metadata.

Example usage:

```
podio-merge-files --output-file merged.root file1.root file2.root file3.root
```

## Converting between ROOT TTrees and RNTuples: `podio-ttree-to-rntuple`

`podio-ttree-to-rntuple` is a command-line tool for converting PODIO files
between ROOT TTree and RNTuple formats. By default, it converts a TTree file to
an RNTuple file. Use the `-r` or `--reverse` option to convert in the opposite
direction (RNTuple to TTree).

You specify the input and output file names as arguments. The tool copies all
categories and entries from the input, preserving the file's full structure.
To see all options and usage instructions, run with `-h`.

Example usage:

```
podio-ttree-to-rntuple input_tree.root output_rntuple.root
podio-ttree-to-rntuple -r input_rntuple.root output_tree.root
```

## Visualizing a model: `podio-vis`

`podio-vis` is a command-line tool to visualize data model descriptions (in
YAML) as graphs using Graphviz. It reads a PODIO YAML data model and generates a
graph where types and their relations are shown.

The main features are:
- Converts a YAML data model to a visual graph (SVG by default, or any Graphviz-supported format with `--fmt`).
- By default, creates the output file based on the `--filename` option (default is `gv`).
- Use `-d`/`--dot` to only save the Graphviz `.dot` source, not the rendered image.
- Supports optional grouping and type filtering with a configuration file (`--graph-conf`).
- Allows for incorporating upstream data models with `--upstream-edm`.
- All relations (one-to-one, one-to-many, and associations) are shown with different visual attributes.
To see all available options and usage instructions, run with `-h`.

Example usage:

```
podio-vis --fmt svg my_model.yaml
firefox gv.svg
```
