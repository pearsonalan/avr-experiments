// Copyright(C) 2020 Alan Pearson
//
// This program is free software : you can redistribute it and /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see < https://www.gnu.org/licenses/>.

#pragma once

struct SimData {
	double gps_alt = 0;
	double gps_lat = 0;
	double gps_lon = 0;
	double gps_track = 0;
	double gps_groundspeed = 0;
	double indicated_airspeed = 0;
	double pitch = 0;
	double bank = 0;
	double heading = 0;
	double heading_bug = 0;
	INT32 landing_light_on = 0;
	INT32 taxi_light_on = 0;
	double gear_left_position = 0;
	double gear_center_position = 0;
	double gear_right_position = 0;
};