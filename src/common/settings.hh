/** \class      Settings
 * @brief      This is the implementation file of Settings 
 * @author     Vicente Adolfo Bolea Sanchez
 * 
 * Settings will read the configuration file eclipse.json and
 * load all the necessary properties. The path of the eclipse.json
 * will be:
 *  -# ~/.eclipse.json
 *  -# /etc/eclipse.json
 *  -# Constructor path @see Settings::Settings(std::string)
 *  -# Hardcoded path, setted using autoconf
 *
 * The way it was designed to be used was:
 *
 * @code
 * Settings setted = Settings().load();
 * string path1 = setted.get<string>("path1");
 * @endcode
 *
 * @attention This class uses the P.I.M.P.L. (Pointer to implementation) idiom
 *            this reduces the complexity of the interface and compilation time. 
 *
 * Copyright © 2015 Vicente Adolfo Bolea Sanchez
 * 
 * This library is free software; you can redistribute it and/or modify
 * It under the terms of the GNU Lesser General Public License as published
 * By the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * But WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * Along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __SETTINGS_HH_
#define __SETTINGS_HH_

#include <string>
#include <memory>

class Settings 
{
  private:
    class SettingsImpl;
    std::unique_ptr<SettingsImpl> impl;

  public:
    Settings();
    Settings(std::string);

    Settings(Settings&&);          //! Move operators
    void operator=(Settings&&);    //!

    ~Settings();

    Settings& load () &;
    Settings&& load () &&;

    template <typename T> T get (std::string) const;
    std::string getip () const;
};

#endif
