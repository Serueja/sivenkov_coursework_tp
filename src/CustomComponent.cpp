#include "CustomComponent.h"

void CustomComponent::display(ImVec2 componentPos = {0,0}, ImVec2 componentSize = {0,0})
{
    componentPos_ = componentPos;
    componentSize_ = componentSize;

    if ( !loginFlag_ )
    {
        loginWindow();
    }
    else
    {
        mainWindow();
    }
}

void CustomComponent::loginWindow()
{
    ImGui::Begin("Login window",
                 nullptr,
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings
                 );

    ImGui::SetWindowPos(componentPos_);
    ImGui::SetWindowSize(componentSize_);

    ImGui::InputText("Username", loginBuf_, IM_ARRAYSIZE(loginBuf_));
    ImGui::InputText("Database path", dbPathBuf_, IM_ARRAYSIZE(dbPathBuf_));
    ImGui::InputText("CSV path", csvPathBuf_, IM_ARRAYSIZE(csvPathBuf_));

    if (ImGui::Button("Login"))
    {
        fs::path tmpCsvPath(csvPathBuf_);
        fs::path tmpDbPath(dbPathBuf_);
        if (!std::filesystem::exists(tmpCsvPath))
        {
            loginWindowError_ = "CSV file not found!";
        }
        else if (!std::filesystem::exists(tmpDbPath))
        {
            loginWindowError_ = "Database file not found!";
        }
        else if ( loginBuf_ == "" )
        {
            loginWindowError_ = "Login not correct!";
        }
        else
        {
            loginFlag_ = true;
            login_ = loginBuf_;
            dbPath_ = dbPathBuf_;
            csvPath_ = csvPathBuf_;
        }
    }

    ImGui::LabelText("", "%s", loginWindowError_.c_str());

    ImGui::End();
}
void CustomComponent::mainWindow()
{
    ImGui::Begin(
            "Main window",
            nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings
    );
    ImGui::SetWindowPos(componentPos_);
    ImGui::SetWindowSize(componentSize_);

    if (ImGui::BeginChild("Info", {componentSize_.x - 20, 120}, true))
    {
        ImGui::LabelText("Username", "%s", login_.c_str());
        ImGui::Separator();
        ImGui::LabelText("Database path", "%s", dbPath_.c_str());
        ImGui::Separator();
        ImGui::LabelText("CSV path", "%s", csvPath_.c_str());
        ImGui::EndChild();
    }
    if ( ImGui::BeginChild("Source", {componentSize_.x - 20, componentSize_.y - 250}, true) )
    {
        if ( !inputInfo_.empty() )
        {
            ImGui::LabelText("Artist", "%s", inputInfo_.front().at(1).c_str());
            ImGui::Separator();
            ImGui::InputTextMultiline("##Text1", 
                (char*)inputInfo_.front().at(2).c_str(), 
                inputInfo_.front().at(2).size(), 
                {componentSize_.x - 65, componentSize_.y - 330}, 
                ImGuiInputTextFlags_ReadOnly
                );
            
            if ( inputInfo_.front().at(2).size() < 150 )
            {
                ImGui::PushStyleColor(ImGuiCol_Text, {0.99, 0.93, 0, 1});
                ImGui::Text( "%s", std::string("Number of characters => " + std::to_string(inputInfo_.front().at(2).size()) + "( < 150 )").c_str());
                ImGui::PopStyleColor();
            }
            else ImGui::Text( "%s", std::string("Number of characters => " + std::to_string(inputInfo_.front().at(2).size())).c_str());
        }
        else ImGui::Text("Конец файла");
        ImGui::EndChild();
    }
    getInfoFromCSV();

    ImGui::Columns(2, nullptr, false);
    if (ImGui::Button("Похож", {componentSize_.x / 2 - 20, 30}))
    {
        writeToDatabase(true);
    }
    ImGui::NextColumn(); 
    if (ImGui::Button("Не похож", {componentSize_.x / 2 - 20, 30}))
    {
        writeToDatabase(false);
    }
    ImGui::Columns(1);
    if (ImGui::Button("Logout", {componentSize_.x - 20, 30}))
    {
        while (!inputInfo_.empty())
        {
            inputInfo_.pop();
        }
        infoWasReceivedFlag_ = false;
        loginFlag_ = false;
    }
    ImGui::End();
}

void CustomComponent::getInfoFromCSV()
{
    if (!infoWasReceivedFlag_)
    {
        while (!inputInfo_.empty())
        {
            inputInfo_.pop();
        }
        std::fstream input;
        input.open(csvPath_, std::ios::in);
        if ( input.is_open() )
        {
            std::string line = "";
            while (std::getline(input, line))
            {
                std::vector<std::string> row;
                std::istringstream lineStream(line);
                std::string cell;

                while (std::getline(lineStream, cell, ';'))
                {
                    for (int i = 0; i < cell.size()-2; i += 1)
                    {
                        if (cell[i] == '.')
                        {
                            cell.insert(cell.begin() + i + 2, '\n');
                        }
                    }
                    row.push_back(cell);
                }

                inputInfo_.push(row);
            }
            infoWasReceivedFlag_ = true;
        }
        else
        {
            ImGui::LabelText("", "File not open");
        }
        input.close();
    }
}

void CustomComponent::writeToDatabase(bool textCharacter)
{
    if (inputInfo_.empty()) return;
    dbError_ = "";
    sqlite3 *db = nullptr;
    char *err = nullptr;

    std::string query = "INSERT INTO table1(model_, author_, text_, user_,textCharacter_) VALUES ('";
    query += inputInfo_.front().at(0);
    query += "', '";
    query += inputInfo_.front().at(1);
    query += "', '";
    query += inputInfo_.front().at(2);
    query += "', '";
    query += login_;
    query += "', ";
    query += std::to_string(textCharacter ? 1 : 0);
    query += ");";

    if ( int tmp = !sqlite3_open(dbPath_.c_str(), &db) )
    {
        if (!sqlite3_exec(db, query.c_str(), nullptr, nullptr, &err) )
        {
            inputInfo_.pop();
        }
        else
        {
            dbError_ = "Error with execute query!";
            std::cout << err << std::endl;
        }
        sqlite3_close(db);
    }
    else
    {
        dbError_ = "Error with open database!";
        std::cout<<tmp<<std::endl;
    }
}

std::vector<BYTE> CustomComponent::base64_decode(std::string const& encoded_string) 
    {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        BYTE char_array_4[4], char_array_3[3];
        std::vector<BYTE> ret;

        while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret.push_back(char_array_3[i]);
            i = 0;
            }
        }

        if (i) {
            for (j = i; j <4; j++)
            char_array_4[j] = 0;

            for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
        }

        return ret;
    }