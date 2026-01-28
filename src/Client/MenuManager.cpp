#include "pch.h"
#include "MenuManager.h"
#include "Menu.h"

void MenuManager::Start()
{
    loadAllMenu();

    for (Menu* object : menuList)
        object->SetActive(false);

    App::GetInstance().CurrentState =  App::State::PLAY;

    switchMenu("PLAY");
    App::GetInstance().Pause();
}

void MenuManager::Update(float deltatime)
{
    m_currentMenu->Update(deltatime);

    switch (App::GetInstance().CurrentState)
    {
    case App::State::PLAY:
    {
        switchMenu("PLAY");
        while (ShowCursor(TRUE) <=0 ); // Boucler jusqu'à ce que le compteur soit négatif
        if(!App::GetInstance().network->Connected)
        {
            getCurrentMenu()->getElement("Play")->visible = false;
        }
        else {
            getCurrentMenu()->getElement("Play")->visible = true;

        }
        App::GetInstance().m_LockCursor =false;
        App::GetInstance().Pause();
        break;
    }
    case App::State::GAME:
    {
        switchMenu("HUD");
        

        break;
    }
    //case App::State::LOOSE:
    //{
    //    switchMenu("LOOSE");
    //    App::GetInstance().Pause();
    //    //gce::ShowMouseCursor();
    //    break;
    //}
    //case App::State::WIN:
    //{
    //    switchMenu("WIN");
    //    App::GetInstance().Pause();
    //    //gce::ShowMouseCursor();
    //    break;
    //}
    //case App::State::PAUSE:
    //{
    //    switchMenu("PAUSE");
    //    App::GetInstance().Pause();
    //    //gce::ShowMouseCursor();
    //    break;
    //}

    default:
        break;
    }

    Menu* Hud = getMenu("HUD");
    if (Hud) {
        UiText* ScoreText = dynamic_cast<UiText*>(Hud->getElement("Score"));
        if (ScoreText)
        {
            ////int score = GameManager::getInstance()->getScore();
            //static std::wstring scoreText;
            //scoreText = std::to_wstring(score);
            //ScoreText->GetComponent<gce::TextRenderer>()->text = scoreText;
        }
    }
}

void MenuManager::Draw( )
{
    if (m_currentMenu && m_currentMenu->active)
        m_currentMenu->Draw();
}

Menu* MenuManager::getMenu(std::string name)
{
    for (Menu* object :menuList)
    {
        if (object->name == name)
        {
            return object;
        }
    }
    return nullptr;
}

void MenuManager::forceUnActiveMenu(std::string name)
{
    for (Menu* object : menuList)
    {
        if (object->name == name)
        {
            object->SetActive(false);
        }
    }
}

void MenuManager::forceActiveMenu(std::string name)
{
    for (Menu* object : menuList)
    {
        if (object->name == name)
        {
            object->SetActive(true);
        }
    }
}

void MenuManager::switchMenu(std::string name)
{
    for (Menu* object : menuList)
    {
        if (object->name == name && object == m_currentMenu)
            return;

        while (ShowCursor(FALSE) >= 0); // Boucler jusqu'à ce que le compteur soit négatif
        App::GetInstance().m_LockCursor = true; // ? AJOUTER CETTE LIGNE !

        object->SetActive(false);
        if (object->name == name)
        {
            if (m_currentMenu != nullptr)
                m_currentMenu->SetActive(false);

            m_currentMenu = object;
            m_currentMenu->SetActive(true);
        }
    }
}

void MenuManager::loadAllMenu()
{
    std::string folderPath = "../../res/Ui/UiMenu";

    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
    {
        if (!entry.is_regular_file())
            continue;
        if (entry.path().extension() != ".json")
            continue;


        std::ifstream file(entry.path());

        if (!file)
        {
            continue;
        }

        nlohmann::json data;
        file >> data;

        std::string menuName = entry.path().stem().string();
        Menu* menu = new Menu();
        menuList.push_back(menu);
        menu->name = menuName.c_str();

        // --------------------
        //      BUTTONS
        // --------------------
        if (data.contains("buttons"))
        {
            for (auto& btn : data["buttons"])
            {
                XMFLOAT3 pos{ btn["pos"]["x"], btn["pos"]["y"], btn["pos"]["z"] };

                XMFLOAT3 size{ btn["size"]["x"], btn["size"]["y"], btn["size"]["z"] };

                std::vector<std::string> listeners;
                if (btn.contains("listenerList"))
                {
                    for (auto& l : btn["listenerList"])
                        listeners.push_back(l.get<std::string>().c_str());
                }

                XMFLOAT2 frameSize{ btn["frameSize"]["x"], btn["frameSize"]["y"] };

                std::string Id = btn["id"];

                menu->addButton(
                    Id,
                    pos,
                    size,
                    btn["name"].get<std::string>(),
                    listeners,
                    btn.contains("hover") ? btn["hover"].get<std::string>().c_str() : "",
                    btn.contains("animated") ? btn["animated"].get<bool>() : false,
                    btn.contains("frameCount") ? btn["frameCount"].get<int>() : 0,
                    btn.contains("currentFrame") ? btn["currentFrame"].get<int>() : 0,
                    frameSize,
                    btn.contains("looping") ? btn["looping"].get<bool>() : false,
                    btn.contains("frameRate") ? btn["frameRate"].get<float>() : 0.0f
                );
            }
        }

        // --------------------
        //       IMAGES
        // --------------------
        if (data.contains("images"))
        {
            for (auto& img : data["images"])
            {
                XMFLOAT3 pos{ img["pos"]["x"], img["pos"]["y"], img["pos"]["z"] };

                XMFLOAT3 size{ img["size"]["x"], img["size"]["y"], img["size"]["z"] };

                XMFLOAT2 frameSize{ img["frameSize"]["x"], img["frameSize"]["y"] };
                std::string Id = img["id"];

                menu->addImage(
                    Id,
                    pos,
                    size,
                    img["name"].get<std::string>().c_str(),
                    img.contains("animated") ? img["animated"].get<bool>() : false,
                    img.contains("frameCount") ? img["frameCount"].get<int>() : 0,
                    img.contains("currentFrame") ? img["currentFrame"].get<int>() : 0,
                    frameSize,
                    img.contains("looping") ? img["looping"].get<bool>() : false,
                    img.contains("frameRate") ? img["frameRate"].get<float>() : 0.0f
                );
            }
        }
        // --------------------
        //        TEXTS
        // --------------------
        if (data.contains("texts"))
        {
            for (auto& txt : data["texts"])
            {
                XMFLOAT3 pos{ txt["pos"]["x"], txt["pos"]["y"], txt["pos"]["z"] };

                XMFLOAT3 size{ txt["size"]["x"], txt["size"]["y"], txt["size"]["z"] };

                XMFLOAT3 color{ txt["color"]["r"], txt["color"]["g"], txt["color"]["b"] };

                std::string text = txt["text"].get<std::string>();
                std::string font = txt["fontPath"].get<std::string>();
                std::string Id = txt["id"];

                int fontSize = txt.contains("fontSize") ? (int)txt["fontSize"].get<float>() : 24;

                menu->addText(
                    Id,
                    pos,
                    size,
                    txt.contains("name") ? txt["name"].get<std::string>() : "TextObject",
                    color,
                    font,
                    text,
                    fontSize
                );
            }
        }

    }
}