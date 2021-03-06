#ifndef ORG_EEROS_HAL_PERIPHERALOUTPUT_HPP_
#define ORG_EEROS_HAL_PERIPHERALOUTPUT_HPP_

#include <eeros/core/System.hpp>
#include <string>

namespace eeros {
	namespace hal {

		class PeripheralOutputInterface {
		public:
			virtual ~PeripheralOutputInterface() { }
			virtual std::string getId() const = 0;
		};

		template <typename T>
		class PeripheralOutput : public PeripheralOutputInterface {
		public:
			explicit PeripheralOutput(std::string id) : id(id) { }
			virtual ~PeripheralOutput() { }
			virtual inline std::string getId() const { return id; }
			virtual T get() = 0;
			virtual void set(T value) = 0;
		private:
			std::string id;
		};

	};
};

#endif /* ORG_EEROS_HAL_PERIPHERALOUTPUT_HPP_ */
