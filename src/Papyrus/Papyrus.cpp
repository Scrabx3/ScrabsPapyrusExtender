#include "Papyrus.h"

#include "Actor.h"
#include "Form.h"
#include "Interface.h"
#include "ObjectRef.h"
#include "Utility.h"

namespace Papyrus
{
	bool Register()
	{
		const auto papyrus = SKSE::GetPapyrusInterface();
		if (!papyrus) {
			logger::critical("Failed to get papyurs interface");
			return false;
		}

		papyrus->Register(Actor::Register);
		papyrus->Register(Form::Register);
		papyrus->Register(Interface::Register);
		papyrus->Register(ObjectRef::Register);
		papyrus->Register(Utility::Register);

		logger::info("Registered Papyrus Functions");

		return true;
	}
}	 // namespace Papyrus