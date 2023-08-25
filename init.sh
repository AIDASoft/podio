# Initialisation script for PODIO build environment
#
# The important variable to come out of the script is PODIO,
# which is needed by some PODIO clients. Usually users will
# set PODIO to the install area for PODIO after it is built,
# but this script provides a "canned" option that can be
# used for bootstrapping and testing.
#
# Script options are:
#  -r - Reset value of PODIO even if it's currently set

# First see if PODIO is already set
if [ -n "$PODIO" -a "$1" != "-r" ]; then
  echo "PODIO already set - use '-r' if you want to reinitialise it"
  return
fi

export PODIO=$(pwd)/install

if [ -e env.sh ]; then
  source ./env.sh
else
  echo "To complete PODIO setup please source the 'env.sh' script"
fi
