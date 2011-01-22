/*
*
 Author Johannes Petrick - 2010

This application is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License 3 as
published by the Free Software Foundation, either version 2.1 of the
License, or (at your option) any later version.

This application is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*
*
*/

/* Prints the Help und Usage of this Programm */
void printUsage(char *prog);

/* init the global config struct */
void init_struct();

/* free the struct and exit with the given exitCode */
void leave(int exitCode);

/* check the given Threshold, print the Temperature and exit with the
   exit code expected by the nagios server */
void check_threshold(int temperature, int warning, int critical, short fahrenheit);

#define VERSION "0.0.9"

