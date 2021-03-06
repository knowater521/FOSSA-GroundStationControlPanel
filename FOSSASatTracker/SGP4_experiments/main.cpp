#include "sgp4io.h"
#include "sgp4unit.h"

#include <GeographicLib/Geocentric.hpp>

#include <iostream>

int main()
{
	/*
		INPUT is e.g.
		2019
		12
		6
		1
		1
		1

		2020
		12
		6
		1
		1
		1

	*/
	char fossasatTLELineA[70] = "1 44829U 19084F   20183.10006475  .00039774  00000-0  23016-3 0  9991";
	char fossasatTLELineB[70] = "2 44829  96.9730  52.2137 0026683 179.4781 180.6520 15.77974288 32594";

	std::cout << "Running! (" << SGP4Version << ")" << std::endl;

	std::cout << "TLE: " << std::endl;
	std::cout << fossasatTLELineA << std::endl;
	std::cout << fossasatTLELineB << std::endl;
	std::cout << std::endl;

	// AIAA-2006-6753-Rev2.pdf (page 50 of 94)
	// Parse the TLE data into the elsetrec structure.
	elsetrec satrec;
	char typerun = 'm';
	char opsmode = 'i';
	char typeinput = 'e';
	gravconsttype gravconst = wgs84;
	double startmfe;
	double stopmfe;
	double deltamin;

	/// @warning stripping const qualifier.
	// This method also invokes "Day2DMYHMS", "JDAY", "SGP4Init".

	/*
	 *  author        : david vallado                  719-573-2600    1 mar 2001
	 *
	 *  inputs        :
	 *    longstr1    - first line of the tle
	 *    longstr2    - second line of the tle
	 *    typerun     - type of run                    verification 'v', catalog 'c',
	 *                                                 manual 'm'
	 *    typeinput   - type of manual input           mfe 'm', epoch 'e', dayofyr 'd'
	 *    opsmode     - mode of operation afspc or improved 'a', 'i'
	 *    whichconst  - which set of constants to use  72, 84
	 *
	 *  outputs       :
	 *    satrec      - structure containing all the sgp4 satellite information
	 */
	twoline2rv((char*)fossasatTLELineA, (char*)fossasatTLELineB,
		typerun, typeinput, opsmode,
		gravconst,
		startmfe, stopmfe, deltamin,
		satrec);

	// Call the propogater (integrator?) at T=0
	double positionVector[3]; // km
	double velocityVector[3]; // km/s
	/*
	*
	*  author        : david vallado                  719-573-2600   28 jun 2005
	*
	*  inputs        :
	*    satrec	 - initialised structure from sgp4init() call.
	*    tsince	 - time eince epoch (minutes)
	*
	*  outputs       :
	*    r           - position vector                     km
	*    v           - velocity                            km/sec
	*  return code - non-zero on error.
	*                   1 - mean elements, ecc >= 1.0 or ecc < -0.001 or a < 0.95 er
	*                   2 - mean motion less than 0.0
	*                   3 - pert elements, ecc < 0.0  or  ecc > 1.0
	*                   4 - semi-latus rectum < 0.0
	*                   5 - epoch elements are sub-orbital
	*                   6 - satellite has decayed
	 */

	// call the propogator once to get the initial state vector value.
	sgp4(gravconst,
		satrec,
		0.0,
		positionVector,
		velocityVector);


	double tsince = startmfe;

	const GeographicLib::Geocentric& ellipsoid = GeographicLib::Geocentric::WGS84();


	if (fabs(tsince) > 1.8e-8)
	{
		tsince = tsince - deltamin;
	}

	while ((tsince < stopmfe) && (satrec.error == 0))
	{
		tsince = tsince + deltamin;

		if (tsince > stopmfe)
		{
			tsince = stopmfe;
		}

		sgp4(gravconst, satrec, tsince, positionVector, velocityVector);

		if (satrec.error > 0)
		{
			std::cout << "Make sure the input is in the form\r\n2020\r\n\12\r\n\6\r\n\1\r\n1\r\n\1" << std::endl;

			printf("Error %f code %3d\r\n", satrec.t, satrec.error);
		}

		if (satrec.error == 0)
		{	
			// convert the position vector from Geocentric to Geodetic.
			double lat, lon, h;
			ellipsoid.Reverse(positionVector[0], positionVector[1], positionVector[2],
				lat, lon, h);

			// get the current datetime.
			double jd = satrec.jdsatepoch + (tsince / 1440.0);
			int year, mon, day, hr, min;
			double sec;
			invjday(jd, year, mon, day, hr, min, sec);


			// print the information %5i%3i%3i %2i:%2i:%9.6f
			std::cout << year << ":" << mon << ":" << day << " " << hr << ":" << min << ":" << sec << std::endl;
			std::cout << std::endl;

			std::cout << "Position Vector (km): " << std::endl;
			std::cout << "(" << positionVector[0] << "," << positionVector[1] << "," << positionVector[2] << ")" << std::endl;
			std::cout << std::endl;

			std::cout << "Position Vector (Lat/Long): " << std::endl;
			std::cout << "(" << lat << ", " << lon << ", " << h << ")" << std::endl;
			std::cout << std::endl;

			std::cout << "Velocity Vector (km/s): " << std::endl;
			std::cout << "(" << velocityVector[0] << "," << velocityVector[1] << "," << velocityVector[2] << ")" << std::endl;
			std::cout << std::endl;
		}
	}


	std::cout << "Finished. Press any key to continue." << std::endl;

	int v;
	std::cin >> v;
	return 0;
}
