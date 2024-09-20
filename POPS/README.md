# POPS Driver
The POPS driver has two roles:

  1: Receive UDP data transmissions from POPS and forward them to telemetry
  2: Provide limit control over startup and shutdown of the POPS control software

As designed, the POPS instrument is an appliance that starts up
whenever power is applied. In the context of a flight mission,
this leaves something to be desired. First, we would like to have
the instrument shut down before or shortly after landing, since
data on the tarmac is generally not the goal. Second, we would
prefer that the instrument not start up post-flight when we want
to power it up to retrieve the flight data. If it does, data taken
in the lab is appended to the flight data.

Data from POPS can be written to a removable USB drive, which
would address the second concern, but flight packaging can make
it difficult to access the USB ports, so retrieving data via
Ethernet can be preferable.

Our solution is to add a POPS_srvr on the POPS BBB, which can
start up the POPS software via a command and also relay a
shutdown command. This requires an separate computer external
to POPS that may be supporting other instruments as well.

The POPS driver in this directory runs on the controlling
computer. It connects to the POPS server at the standard POPS
IP address 10.11.97.50 using the port for the "pops" service
defined in the instrument-local services file. Both the IP
address and the port will need to be made configurable when
operating more than one POPS instrument. The driver will use
the same port number to receive UDP data from POPS.

POPS listens for commands on the same UDP ports it sends to
(but not on the broadcast channel, the first definition in the
list). This does not interfere with POPS_srvr using the same
port number to listen on TCP.

POPS Viewer defaults to 10.11.97.50 port 10080, which corresponds
with the second UDP channel defined. I think I will disable the
other channels and ensure my configurations line up with that.
Configuration changes need to be made in:

  - pops:/media/uSD/POPS_BBB.cfg: remote IP, port
  - pops:/etc/network/interfaces: instrument's IP address
  - src/TM/services: port definitions
  - src/TM/interact: IP address (not implemented yet)
  - POPSViewer.ini

The original versions of all configuration files on the POPS BBB
that require modification are saved in src/BBB/dist. The
customized versions are saved in src/BBB/cust. Files that need
to be different on different POPS instruments will be placed
in subdirectories by abbreviated hostname.

Changes required when moving to a new network:

  - Update POPS IP in cust/$tgthost/interfaces
  - Update host IP in cust/$tgthost/POPS_BBB.cfg

Additional changes required when adding a second POPS instrument:
  - Duplicate config files from cust/pops
  - Update POPS_srvr command line in cust/$tgthost/pops_server.service