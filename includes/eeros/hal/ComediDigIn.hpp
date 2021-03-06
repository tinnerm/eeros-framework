#ifndef ORG_EEROS_HAL_COMEDIDIGIN_HPP_
#define ORG_EEROS_HAL_COMEDIDIGIN_HPP_

#include <string>
#include <comedilib.h>
#include <eeros/hal/PeripheralInput.hpp>
#include <eeros/hal/ComediDevice.hpp>

namespace eeros {
	namespace hal {

		class ComediDigIn : public PeripheralInput<bool> {
		public:
			ComediDigIn(std::string id, ComediDevice* device, uint32_t subDeviceNumber, uint32_t channel, bool inverted = false);
			virtual bool get();
			
		private:
			comedi_t* deviceHandle;
			uint32_t subDeviceNumber;
			uint32_t channel;
			
			bool inverted;
		};

	};
};

#endif /* ORG_EEROS_HAL_COMEDIDIGIN_HPP_ */
