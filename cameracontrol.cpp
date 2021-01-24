#include "predef.h"
#include "cameracontrol.h"
#include "utils.h"
#include <iostream>
#include <functional>


CameraControl::CameraControl()
{
	cameraWrapper = nullptr;
}


CameraControl::~CameraControl()
{
	if(cameraWrapper != nullptr)
		delete cameraWrapper;
}

bool CameraControl::Initialize(string modelname, string port)
{
	cameraWrapper = new gphoto2pp::CameraWrapper(modelname, port);
	return true;
}

void CameraControl::SetParam(CAMERA_PARAM param, string value)
{
	params[param] = value;
}

bool CameraControl::ReleaseButton()
{
	if (SetRadioWidgetName("eosremoterelease", "Release 1") == false)
		return false;

	return true;
}

bool CameraControl::AutoFocus()
{
	if (SetRadioWidgetName("eosremoterelease", "Release 1") == false)
	 	return false;

	if (SetRadioWidgetName("eosremoterelease", "Press 1") == false)
		return false;

	Utils::Sleep(1);

	if (SetToggleWidget("autofocusdrive", "false") == false)
		return false;

	if (SetToggleWidget("cancelautofocus", "true") == false)
		return false;

	return true;
}

bool CameraControl::ApplyParam()
{
	if (SetRadioWidget("ISO Speed", params[CAMERA_PARAM::ISO]) == false) return false;
	if (SetRadioWidget("Aperture", params[CAMERA_PARAM::APERTURE]) == false) return false;
	if (SetRadioWidget("Shutter Speed", params[CAMERA_PARAM::SHUTTERSPEED]) == false) return false;
	if (SetRadioWidget("Image Format", params[CAMERA_PARAM::CAPTURE_FORMAT]) == false) return false;

	Logger::log("ApplyParam");
	return true;
}

bool  CameraControl::SetToggleWidget(string param, string value)
{
	auto rootWidget = cameraWrapper->getConfig();
	auto toggleWidget = rootWidget.getChildByName<gphoto2pp::ToggleWidget>(param.c_str());

// 	std::cout << "--> label: " << toggleWidget.getLabel() << std::endl;
// 	std::cout << "--> info:  " << toggleWidget.getInfo() << std::endl;
// 	std::cout << "--> type: " << static_cast<int>(toggleWidget.getType()) << std::endl;
// 	std::cout << "--> value: " << toggleWidget.getValue() << std::endl;

	try
	{
		toggleWidget.setValue((value == "true" ? 1 : 0));
		cameraWrapper->setConfig(rootWidget);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "\tError: couldn't change the SetRadioWidget" << std::endl;
		return false;
	}
	catch (gphoto2pp::exceptions::ValueOutOfLimits& e)
	{
		std::cout << "\tError: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool  CameraControl::SetRadioWidget(string param, string value)
{
	auto rootWidget = cameraWrapper->getConfig();
	auto radioWidget = rootWidget.getChildByLabel<gphoto2pp::RadioWidget>(param.c_str());

// 	std::cout << "\t   label:\t" << radioWidget.getLabel() << std::endl;
// 	std::cout << "\t   info:\t " << radioWidget.getInfo() << std::endl;
// 	std::cout << "\t   type:\t" << static_cast<int>(radioWidget.getType()) << std::endl;
//	std::cout << "\t   value:\t" << radioWidget.getValue() << std::endl;
// 	std::cout << "\t   count:\t" << radioWidget.countChoices() << std::endl;
// 	std::cout << "\t   choices:\t" << radioWidget.choicesToString(" ") << std::endl;

	try
	{
		radioWidget.setValue(value.c_str());
		cameraWrapper->setConfig(rootWidget);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "\tError: couldn't change the SetRadioWidget" << std::endl;
		return false;
	}
	catch (gphoto2pp::exceptions::ValueOutOfLimits& e)
	{
		std::cout << "\tError: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool  CameraControl::SetRadioWidgetName(string param, string value)
{
	auto rootWidget = cameraWrapper->getConfig();
	auto radioWidget = rootWidget.getChildByName<gphoto2pp::RadioWidget>(param.c_str());

// 	std::cout << "\t   label:\t" << radioWidget.getLabel() << std::endl;
// 	std::cout << "\t   info:\t " << radioWidget.getInfo() << std::endl;
// 	std::cout << "\t   type:\t" << static_cast<int>(radioWidget.getType()) << std::endl;
// 	std::cout << "\t   value:\t" << radioWidget.getValue() << std::endl;
// 	std::cout << "\t   count:\t" << radioWidget.countChoices() << std::endl;
// 	std::cout << "\t   choices:\t" << radioWidget.choicesToString(" ") << std::endl;

	try
	{
		radioWidget.setValue(value.c_str());
		cameraWrapper->setConfig(rootWidget);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "\tError: couldn't change the SetRadioWidgetName" << std::endl;
		return false;
	}
	catch (gphoto2pp::exceptions::ValueOutOfLimits& e)
	{
		std::cout << "\tError: " << e.what() << std::endl;
		return false;
	}

	return true;
}

int CameraControl::GetFilefromCamera(const char* filename)
{
	Camera* _camera = (Camera*)cameraWrapper->getCamera();
	GPContext* _context = (GPContext*)cameraWrapper->getContext();

	void* data = NULL;
	CameraEventType	event;
	CameraFilePath* fn;
	int ret;
	bool loop = true;

	while (loop)
	{
		int leftoverms = 1000;
		ret = gp_camera_wait_for_event(_camera, leftoverms, &event, &data, _context);
		if (ret != GP_OK)
			return ret;

		switch (event)
		{
			case GP_EVENT_UNKNOWN:
			case GP_EVENT_TIMEOUT:
			case GP_EVENT_FOLDER_ADDED:
			case GP_EVENT_FILE_CHANGED:
				break;

			case GP_EVENT_FILE_ADDED:
			{
				fn = (CameraFilePath*)data;

				CameraFile* file;
				CameraFileInfo info;
				ret = gp_camera_file_get_info(_camera, fn->folder, fn->name, &info, _context);
				if (ret != GP_OK)
				{
					Logger::log("gp_camera_file_get_info %s : %d Error", filename, ret);

					free(data);
					return ret;
				}
				else
				{
					Logger::log("gp_camera_file_get_info %s : %s success", fn->folder, fn->name);
				}

				int fd;
				fd = open(filename, O_CREAT | O_RDWR, 0644);
				ret = gp_file_new_from_fd(&file, fd);
				if (ret != GP_OK)
				{
					Logger::log("gp_file_new_from_fd %s Error", filename);
					gp_file_unref(file);
					free(data);
					return ret;
				}

				ret = gp_camera_file_get(_camera, fn->folder, fn->name, GP_FILE_TYPE_NORMAL, file, _context);
				if (ret != GP_OK)
				{
					Logger::log("gp_camera_file_get %s : %d Error", filename, ret);
					gp_file_unref(file);
					free(data);
					return ret;
				}
				else
				{
					Logger::log("gp_camera_file_get %s : %s success", fn->folder, fn->name);
				}

				//gp_file_unref(file);
				gp_file_free(file);
				//loop = false;
			}
			break;

			case GP_EVENT_CAPTURE_COMPLETE:
			{
				loop = false;
				Logger::log("Capture %s Done!", filename);
			}
			break;
		}
		free(data);
	}

	return true;
}

int CameraControl::_find_widget_by_name(GPParams* p, const char* name, CameraWidget** child, CameraWidget** rootconfig)
{
	int	ret;

	*rootconfig = NULL;
	ret = gp_camera_get_single_config(p->camera, name, child, p->context);
	if (ret == GP_OK) 
	{
		*rootconfig = *child;
		return GP_OK;
	}

	ret = gp_camera_get_config(p->camera, rootconfig, p->context);
	if (ret != GP_OK) return ret;
	ret = gp_widget_get_child_by_name(*rootconfig, name, child);
	if (ret != GP_OK)
		ret = gp_widget_get_child_by_label(*rootconfig, name, child);
	if (ret != GP_OK)
	{
		char* part, * s, * newname;

		newname = strdup(name);
		if (!newname)
			return GP_ERROR_NO_MEMORY;

		*child = *rootconfig;
		part = newname;
		while (part[0] == '/')
			part++;
		while (1)
		{
			CameraWidget* tmp;

			s = strchr(part, '/');
			if (s)
				*s = '\0';
			ret = gp_widget_get_child_by_name(*child, part, &tmp);
			if (ret != GP_OK)
				ret = gp_widget_get_child_by_label(*child, part, &tmp);
			if (ret != GP_OK)
				break;
			*child = tmp;
			if (!s)
			{
				/* end of path */
				free(newname);
				return GP_OK;
			}
			part = s + 1;
			while (part[0] == '/')
				part++;
		}
		gp_context_error(p->context, "%s not found in configuration tree.", newname);
		free(newname);
		gp_widget_free(*rootconfig);
		return GP_ERROR;
	}
	return GP_OK;
}


int CameraControl::SetConfigAction(const char* name, const char* value)
{
	GPParams p;
	p.camera = (Camera*)cameraWrapper->getCamera();
	p.context = (GPContext*)cameraWrapper->getContext();

	CameraWidget* rootconfig, * child;
	int	ret, ro;
	CameraWidgetType	type;

	ret = _find_widget_by_name(&p, name, &child, &rootconfig);
	if (ret != GP_OK)
		return ret;

	ret = gp_widget_get_readonly(child, &ro);
	if (ret != GP_OK)
	{
		gp_widget_free(rootconfig);
		return ret;
	}
	if (ro == 1)
	{
		gp_context_error(p.context, "Property %s is read only.", name);
		gp_widget_free(rootconfig);
		return GP_ERROR;
	}
	ret = gp_widget_get_type(child, &type);
	if (ret != GP_OK) {
		gp_widget_free(rootconfig);
		return ret;
	}

	switch (type)
	{
	case GP_WIDGET_TEXT:
	{		/* char *		*/
		ret = gp_widget_set_value(child, value);
		if (ret != GP_OK)
			gp_context_error(p.context, "Failed to set the value of text widget %s to %s.", name, value);
		break;
	}
	case GP_WIDGET_RANGE:
	{	/* float		*/
		float	f, t, b, s;

		ret = gp_widget_get_range(child, &b, &t, &s);
		if (ret != GP_OK)
			break;
		if (!sscanf(value, "%f", &f)) {
			gp_context_error(p.context, "The passed value %s is not a floating point value.", value);
			ret = GP_ERROR_BAD_PARAMETERS;
			break;
		}
		if ((f < b) || (f > t)) {
			gp_context_error(p.context, "The passed value %f is not within the expected range %f - %f.", f, b, t);
			ret = GP_ERROR_BAD_PARAMETERS;
			break;
		}
		ret = gp_widget_set_value(child, &f);
		if (ret != GP_OK)
			gp_context_error(p.context, "Failed to set the value of range widget %s to %f.", name, f);
		break;
	}
	case GP_WIDGET_TOGGLE:
	{	/* int		*/
		int	t;

		t = 2;
		if (!strcasecmp(value, "off") || !strcasecmp(value, "no") ||
			!strcasecmp(value, "false") || !strcmp(value, "0") ||
			!strcasecmp(value, "off") || !strcasecmp(value, "no") ||
			!strcasecmp(value, "false")
			)
			t = 0;
		if (!strcasecmp(value, "on") || !strcasecmp(value, "yes") ||
			!strcasecmp(value, "true") || !strcmp(value, "1") ||
			!strcasecmp(value, "on") || !strcasecmp(value, "yes") ||
			!strcasecmp(value, "true")
			)
			t = 1;
		/*fprintf (stderr," value %s, t %d\n", value, t);*/
		if (t == 2) {
			gp_context_error(p.context, "The passed value %s is not a valid toggle value.", value);
			ret = GP_ERROR_BAD_PARAMETERS;
			break;
		}
		ret = gp_widget_set_value(child, &t);
		if (ret != GP_OK)
			gp_context_error(p.context, "Failed to set values %s of toggle widget %s.", value, name);
		break;
	}

	case GP_WIDGET_DATE:
	{		/* int			*/
		time_t	t = -1;
		struct tm xtm;

		memset(&xtm, 0, sizeof(xtm));

		/* We need to set UNIX time in seconds since Epoch */
		/* We get ... local time */

		if (!strcasecmp(value, "now") || !strcasecmp(value, "now"))
			t = time(NULL);
#ifdef HAVE_STRPTIME
		else if (strptime(value, "%c", &xtm) || strptime(value, "%Ec", &xtm)) {
			xtm.tm_isdst = -1;
			t = mktime(&xtm);
		}
#endif
		if (t == -1)
		{
			unsigned long lt;

			if (!sscanf(value, "%ld", &lt))
			{
				gp_context_error(p.context, "The passed value %s is neither a valid time nor an integer.", value);
				ret = GP_ERROR_BAD_PARAMETERS;
				break;
			}
			t = lt;
		}
		ret = gp_widget_set_value(child, &t);
		if (ret != GP_OK)
			gp_context_error(p.context, "Failed to set new time of date/time widget %s to %s.", name, value);
		break;
	}
	case GP_WIDGET_MENU:
	case GP_WIDGET_RADIO:
	{ /* char *		*/
		int cnt, i;
		char* endptr;

		cnt = gp_widget_count_choices(child);
		if (cnt < GP_OK) 
		{
			ret = cnt;
			break;
		}
		ret = GP_ERROR_BAD_PARAMETERS;
		for (i = 0; i < cnt; i++) 
		{
			const char* choice;

			ret = gp_widget_get_choice(child, i, &choice);
			if (ret != GP_OK)
				continue;
			if (!strcmp(choice, value)) 
			{
				ret = gp_widget_set_value(child, value);
				break;
			}
		}
		if (i != cnt)
			break;

		/* make sure we parse just 1 integer, and there is nothing more.
		 * sscanf just does not provide this, we need strtol.
		 */
		i = strtol(value, &endptr, 10);
		if ((value != endptr) && (*endptr == '\0')) 
		{
			if ((i >= 0) && (i < cnt)) 
			{
				const char* choice;

				ret = gp_widget_get_choice(child, i, &choice);
				if (ret == GP_OK)
					ret = gp_widget_set_value(child, choice);
				break;
			}
		}
		/* Lets just try setting the value directly, in case we have flexible setters,
		 * like PTP shutterspeed. */
		ret = gp_widget_set_value(child, value);
		if (ret == GP_OK)
			break;
		gp_context_error(p.context, "Choice %s not found within list of choices.", value);
		break;
	}

	/* ignore: */
	case GP_WIDGET_WINDOW:
	case GP_WIDGET_SECTION:
	case GP_WIDGET_BUTTON:
		gp_context_error(p.context, "The %s widget is not configurable.", name);
		ret = GP_ERROR_BAD_PARAMETERS;
		break;
	}
	if (ret == GP_OK)
	{
		if (child == rootconfig)
			ret = gp_camera_set_single_config(p.camera, name, child, p.context);
		else
			ret = gp_camera_set_config(p.camera, rootconfig, p.context);
		if (ret != GP_OK)
			gp_context_error(p.context, "Failed to set new configuration value %s for configuration entry %s.", value, name);
	}
	gp_widget_free(rootconfig);
	return (ret);
}

