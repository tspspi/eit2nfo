# Simple EIT to NFO converter

This is a simple (read: fast hacked with not many features) converter for EIT files generated
by some DVB receivers that include video information for recorded MPEG datastreams. To allow
easy automatic indexing of these files by solutions like XMBC/Kodi one has to provide metadata
in a format that these solution understands - in this case NFO files.

This program allows to create NFOs that contain just the title and an outline - the title
is extracted from the EIT from the first `short_event_descriptor`, the outline
is built by concatenation of all `extended_event_descriptor`s found. Currently only
default characterset, UTF-8 and ISO8859-9 payloads are correctly decoded - for other encodings
corresponding translation tables would have to be added.

The translator allows to convert:

* a single file with specified source and destination: `eit2nfo sourcefile destfile`
* a single file with an automatically generated destination name: `eit2nfo sourcefile.eit`
* A whole directory tree. If one specifies only a target directory all subdirectories and all files are crawled, all `*.eit` files are located and if there is no corresponding `*.nfo` file or the NFO is older than the EIT the NFO is automatically generated

The translator checks if an NFO already exists and only overwrites NFOs in case they are older than the EIT visited. One can disable this behaviour by setting the `-o` flag

## License

This program is distributed under an simplified BSD license, see the `LICENSE` file. Basically do what you want as long as you don't blame anybody.
