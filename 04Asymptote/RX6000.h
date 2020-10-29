#pragma once

#include "Beaker.h"

/* Today is October 29nd, 2020,
   release date for the RX6000 series of AMD.
   */
class RX6000
{
private:
	// Each beaker holds an image
	std::vector<std::unique_ptr<Beaker>> m_vuptrBeakers;
};

