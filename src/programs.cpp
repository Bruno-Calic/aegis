#include <iostream>
#include "aegis/Scenario.hpp"
#include "aegis/simulation/SimulationEngine.hpp"
#include "aegis/pathfinding/PathFinder.hpp"
#include <iomanip>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <implot.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>

void printHelloWorld() {
    std::cout << "Hello, World!" << std::endl;
}


int test_1() {
    using namespace aegis;

    // 1. Stvori zgradu: Soba1 -> Soba2 -> Soba3 (Izlaz)
    Scenario scenario;
    scenario.name = "JednostavanTest";

    // Čvorovi
    Node n1{1, NodeType::Room, "Soba1", 0, 0};
    Node n2{2, NodeType::Room, "Soba2", 1, 0};
    Node n3{3, NodeType::Exit, "Soba3_Izlaz", 2, 0};

    scenario.nodes = {n1, n2, n3};

    // Bridovi
    Edge e1{10, 1, 2, 5.0, 1.5, 5.0, 10, false, false};
    Edge e2{11, 2, 3, 5.0, 1.5, 5.0, 10, false, false};

    scenario.edges = {e1, e2};

    // 2. Dodaj 1 agenta u Sobu1
    Agent a1;
    a1.id = 1;
    a1.currentNode = 1;
    a1.state = AgentState::Initial;
    scenario.agents.push_back(a1);

    // 3. Pokreni simulaciju
    SimulationEngine engine;
    engine.initialize(scenario);

    std::cout << "=== Aegis Simulacija ===\n";
    std::cout << "Scenarij: " << scenario.name << "\n";
    std::cout << "Agent kreće iz čvora 1 (Soba1)\n";
    std::cout << "Cilj: čvor 3 (Soba3_Izlaz)\n";
    std::cout << "========================\n\n";

    int korak = 0;
    
    // Ispiši početno stanje
    std::cout << "KORAK  0 | Vrijeme: 0.00 s | Agent " 
              << scenario.agents[0].id 
              << " je na čvoru " << scenario.agents[0].currentNode << "\n";

    while (!engine.isFinished()) {
        korak++;
        
        // Pokreni korak
        engine.step(0.25);
        
        // Ispiši stanje NAKON koraka
        const auto& state = engine.state();
        const auto& agent = state.agents[0];
        
        std::cout << "KORAK " << std::setw(2) << korak 
                  << " | Vrijeme: " << std::setw(6) << std::fixed << std::setprecision(2) << state.currentTime << " s"
                  << " | Agent " << agent.id;
        
        if (agent.state == AgentState::Evacuated) {
            std::cout << " -> EVAKUIRAN!\n";
        } else if (agent.state == AgentState::Trapped) {
            std::cout << " -> ZAROBLJEN!\n";
        } else {
            std::cout << " je na čvoru " << agent.currentNode << "\n";
        }
    }

    // 4. Konačni rezultati
    const auto& s = engine.state();
    std::cout << "\n========================\n";
    std::cout << "SIMULACIJA ZAVRŠENA\n";
    std::cout << "Ukupno agenata: " << scenario.agents.size() << "\n";
    std::cout << "Evakuirano: " << s.evacuatedCount << "\n";
    std::cout << "Zarobljeno: " << s.trappedCount << "\n";
    std::cout << "Ukupno vrijeme: " << s.currentTime << " s\n";
    std::cout << "Ukupno koraka: " << korak << "\n";
    std::cout << "========================\n";

    return 0;
}

int test_2(){
    using namespace aegis;

        
    //stvorit scenarij
    Scenario scenario;
    scenario.name = "Test2";


    //dodaj čvorove
    Node n1{1, NodeType::Room, "Informatika", 0, 0};
    Node n2{2, NodeType::Corridor, "Hodnik", 1, 0};
    Node n3{3, NodeType::Staircase, "stepenice", 1, 1};
    Node n4{4, NodeType::Room, "Razredna", 2, 1};
    Node n5{5, NodeType::Staircase, "stepenice", 2, 2};
    Node n6{6, NodeType::Exit, "Izlaz", 2, 3}; 


    //Scenario cvorovi
    scenario.nodes = {n1, n2, n3, n4, n5, n6};


    //dodavanje zidovi
    Edge e1{1, 1, 2, 2.0, 1.0, 0.5, 2, false, false};
    Edge e2{2, 2, 3, 8.0, 4.0, 2.0, 6, false, false};
    Edge e3{3, 3, 4, 6.0, 4.0, 2.3, 5, false, false};
    Edge e4{4, 4, 5, 6.0, 4.0, 1.5, 4, false, false};
    Edge e5{5, 5, 6, 6.0, 4.0, 2.0, 5, false, true};


    //Scenario zidove
    scenario.edges = {e1, e2, e3, e4, e5};


    //dodavanje agenata
    Agent a1;
    a1.id = 1;
    a1.currentNode = 1;
    a1.state = AgentState::Initial;
    Agent a2;
    a2.id = 2;
    a2.currentNode = 2;
    a2.state = AgentState::Initial;
    Agent a3;
    a2.id = 2;
    a2.currentNode = 2;
    a2.state = AgentState::Initial;


    //Scenario agenti
    scenario.agents = {a1, a2};


        //pokretanje simulacije
    SimulationEngine engine;
    engine.initialize(scenario);

    std::cout << "=== Aegis Simulacija ===\n";
    std::cout << "Scenarij: " << scenario.name << "\n";
    std::cout << "========================\n\n";


    //simulacija i obrada
    int korak = 0;
    while (!engine.isFinished()) {
        korak++;
        
        // Pokreni korak
        engine.step(0.25);
        
        // Ispiši stanje
        std::cout << "KORAK " << korak << " | Vrijeme: " 
                << engine.state().currentTime << " s\n";
        
        // Ispiši poziciju agenata
        for (const auto& agent : engine.state().agents) {

            if (agent.currentEdge != -1) {
                std::cout << "  Agent " << agent.id << " je NA BRIDU " << agent.currentEdge << "\n";
            }
            else {
                std::cout << "  Agent " << agent.id << " je na čvoru " << agent.currentNode << "\n";
            }
            if (agent.state == AgentState::Evacuated) {
                std::cout << " [EVAKUIRAN]";
            } else if (agent.state == AgentState::Trapped) {
                std::cout << " [ZAROBLJEN]";
            } else if (agent.state == AgentState::Waiting) {
                std::cout << " [ČEKA]";
            } else {
                std::cout << " [KREĆE SE]";
            }
            
            std::cout << "\n";
        }
        
        // Ispiši gužvu na bridovima
        std::cout << "  Gužva:\n";
        for (const auto& edge : engine.state().edges) {
            auto congestion = engine.getEdgeCongestion(edge.id);
            std::cout << "    Brid " << edge.id << ": " 
                    << congestion.occupancy << "/" << congestion.capacity 
                    << " (gustoća: " << congestion.density << ")\n";
        }
        
        std::cout << "\n";
    }


    //OBRADA//


    if(engine.isFinished()){
        std::cout << "Simulacija je završila.\n";
        std::cout << "Ukupno vrijeme: " << engine.state().currentTime << " s\n";
        std::cout << "Evakuirano: " << engine.state().evacuatedCount << "\n";
        std::cout << "Zarobljeno: " << engine.state().trappedCount << "\n";
        std::cout << "Ukupno koraka: " << korak << "\n";
        
    } else {
        std::cout << "Simulacija još uvijek traje.\n";
    }


    return 0;
}


int test_window() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Aegis Viewer", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Aegis Simulacija");
        ImGui::Text("Ovdje ce doci canvas s grafom");
        if (ImGui::Button("Korak")) { /* engine.step(0.25) */ }
        ImGui::End();

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

int test_4() {
    using namespace aegis;

    Scenario scenario;
    scenario.name = "Test4_Tlocrt";

    Node n1{1, NodeType::Room,      "Informatika", 0, 0};
    Node n2{2, NodeType::Corridor,  "Hodnik",      1, 0};
    Node n3{3, NodeType::Staircase, "Stepenice1",  1, 1};
    Node n4{4, NodeType::Room,      "Razredna",    2, 1};
    Node n5{5, NodeType::Staircase, "Stepenice2",  2, 2};
    Node n6{6, NodeType::Exit,      "Izlaz",       2, 3};
    Node n7{7, NodeType::Room,      "Soba1",       0, 1};

    scenario.nodes = {n1, n2, n3, n4, n5, n6, n7};

    Edge e1{1, 1, 2, 2.0, 1.0, 0.5, 2, false, false};
    Edge e2{2, 2, 3, 2.0, 4.0, 2.0, 6, false, false};
    Edge e3{3, 3, 4, 6.0, 4.0, 2.3, 5, false, false};
    Edge e4{4, 4, 5, 6.0, 4.0, 1.5, 4, false, false};
    Edge e5{5, 5, 6, 6.0, 4.0, 2.0, 5, false, false};
    Edge e6{6, 1, 7, 2.0, 1.0, 2.0, 2, false, false}; 
    Edge e7{7, 7, 3, 2.0, 1.0, 0.5, 2, false, false}; 

    scenario.edges = {e1, e2, e3, e4, e5, e6, e7};

    Agent a1;
    a1.id = 1;
    a1.currentNode = 1;
    a1.state = AgentState::Initial;

    Agent a2;
    a2.id = 2;
    a2.currentNode = 1;
    a2.state = AgentState::Initial;

    Agent a3;
    a3.id = 3;
    a3.currentNode = 1;
    a3.state = AgentState::Initial;

    Agent a4;
    a4.id = 4;
    a4.currentNode = 1;
    a4.state = AgentState::Initial;

    Agent a5;
    a5.id = 5;
    a5.currentNode = 1;
    a5.state = AgentState::Initial;

    scenario.agents = {a1, a2, a3, a4, a5};

    SimulationEngine engine;
    engine.initialize(scenario);

    if (!glfwInit()) {
        std::cerr << "Ne mogu pokrenuti GLFW.\n";
        return -1;
    }

    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(1400, 900, "Aegis - Test 4 Tlocrt", nullptr, nullptr);
    if (!window) {
        std::cerr << "Ne mogu stvoriti prozor.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool autoRun = false;
    float autoAccumulator = 0.0f;

    auto resetSimulation = [&]() {
        engine.initialize(scenario);
        autoRun = false;
        autoAccumulator = 0.0f;
    };

    auto nodePosFromState = [&](const SimulationState& state, int id, ImVec2 origin, float cell) -> ImVec2 {
        for (const auto& n : state.nodes) {
            if (n.id == id) {
                return ImVec2(
                    origin.x + 120.0f + static_cast<float>(n.x) * cell,
                    origin.y + 100.0f + static_cast<float>(n.y) * cell
                );
            }
        }
        return origin;
    };

    auto findEdgeInState = [&](const SimulationState& state, int edgeId) -> const Edge* {
        for (const auto& e : state.edges) {
            if (e.id == edgeId) return &e;
        }
        return nullptr;
    };

    auto congestionColor = [&](double density) -> ImU32 {
        float d = static_cast<float>(density);
        int r = static_cast<int>(60 + 190 * d);
        int g = static_cast<int>(170 - 110 * d);
        int b = static_cast<int>(90 - 40 * d);
        return IM_COL32(r, g, b, 190);
    };

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (autoRun && !engine.isFinished()) {
            autoAccumulator += io.DeltaTime;
            if (autoAccumulator >= 0.25f) {
                engine.step(0.25);
                autoAccumulator = 0.0f;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const SimulationState& state = engine.state();

        ImGui::SetNextWindowPos(ImVec2(15, 15), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(320, 260), ImGuiCond_FirstUseEver);
        ImGui::Begin("Kontrole");

        ImGui::Text("Scenarij: %s", scenario.name.c_str());
        ImGui::Separator();
        ImGui::Text("Vrijeme: %.2f s", state.currentTime);
        ImGui::Text("Evakuirano: %d", state.evacuatedCount);
        ImGui::Text("Zarobljeno: %d", state.trappedCount);

        if (ImGui::Button("Korak", ImVec2(90, 32))) {
            if (!engine.isFinished()) {
                engine.step(0.25);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(autoRun ? "Stop" : "Auto", ImVec2(90, 32))) {
            autoRun = !autoRun;
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset", ImVec2(90, 32))) {
            resetSimulation();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Status agenata:");

        for (const auto& agent : state.agents) {
            const char* status = "Initial";
            if (agent.state == AgentState::Waiting) status = "Ceka";
            else if (agent.state == AgentState::Moving) status = "Krece se";
            else if (agent.state == AgentState::Evacuated) status = "Evakuiran";
            else if (agent.state == AgentState::Trapped) status = "Zarobljen";
            else if (agent.state == AgentState::Blocked) status = "Blokiran";

            if (agent.currentEdge != -1) {
                ImGui::Text("Agent %d | %s | edge=%d | rem=%.2f",
                            agent.id, status, agent.currentEdge, agent.remainingEdgeTime);
            } else {
                ImGui::Text("Agent %d | %s | node=%d",
                            agent.id, status, agent.currentNode);
            }
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(350, 15), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1020, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Tlocrt");

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 origin = ImGui::GetCursorScreenPos();
        const float cell = 170.0f;

        auto nodePos = [&](int id) -> ImVec2 {
            return nodePosFromState(state, id, origin, cell);
        };

        // Hodnici
        for (const auto& e : state.edges) {
            ImVec2 p1 = nodePos(e.from);
            ImVec2 p2 = nodePos(e.to);

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len < 1e-5f) continue;

            float nx = -dy / len;
            float ny =  dx / len;

            float halfWidth = 14.0f + static_cast<float>(e.width) * 4.0f;

            ImVec2 a(p1.x + nx * halfWidth, p1.y + ny * halfWidth);
            ImVec2 b(p2.x + nx * halfWidth, p2.y + ny * halfWidth);
            ImVec2 c(p2.x - nx * halfWidth, p2.y - ny * halfWidth);
            ImVec2 d(p1.x - nx * halfWidth, p1.y - ny * halfWidth);

            auto cg = engine.getEdgeCongestion(e.id);

            dl->AddQuadFilled(a, b, c, d, congestionColor(cg.density));
            dl->AddLine(a, b, IM_COL32(235, 235, 235, 255), 3.0f);
            dl->AddLine(d, c, IM_COL32(235, 235, 235, 255), 3.0f);

            ImVec2 mid((p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f);

            char occBuf[32];
            std::snprintf(occBuf, sizeof(occBuf), "%d/%d", cg.occupancy, cg.capacity);
            dl->AddText(ImVec2(mid.x - 12.0f, mid.y - 10.0f), IM_COL32_WHITE, occBuf);

            char edgeBuf[16];
            std::snprintf(edgeBuf, sizeof(edgeBuf), "E%d", e.id);
            dl->AddText(ImVec2(mid.x - 10.0f, mid.y + 8.0f), IM_COL32(220, 220, 220, 255), edgeBuf);
        }

        // Čvorovi
        for (const auto& n : state.nodes) {
            ImVec2 p = nodePos(n.id);

            ImU32 fill = IM_COL32(120, 120, 120, 255);
            if (n.type == NodeType::Exit) {
                fill = IM_COL32(60, 140, 255, 255);
            } else if (n.type == NodeType::Staircase) {
                fill = IM_COL32(175, 120, 235, 255);
            } else if (n.type == NodeType::Corridor) {
                fill = IM_COL32(110, 160, 110, 255);
            } else if (n.type == NodeType::Room) {
                fill = IM_COL32(150, 150, 150, 255);
            }

            dl->AddCircleFilled(p, 17.0f, fill);
            dl->AddCircle(p, 17.0f, IM_COL32(255, 255, 255, 255), 0, 2.0f);

            char idBuf[16];
            std::snprintf(idBuf, sizeof(idBuf), "%d", n.id);
            dl->AddText(ImVec2(p.x - 4.0f, p.y - 7.0f), IM_COL32_BLACK, idBuf);

            dl->AddText(ImVec2(p.x - 35.0f, p.y + 24.0f), IM_COL32_WHITE, n.name.c_str());
        }

        // Agenti
        int movingLane = 0;

        for (const auto& agent : state.agents) {
            ImU32 col = IM_COL32(255, 170, 0, 255);
            if (agent.state == AgentState::Evacuated) {
                col = IM_COL32(80, 220, 100, 255);
            } else if (agent.state == AgentState::Waiting) {
                col = IM_COL32(255, 90, 90, 255);
            } else if (agent.state == AgentState::Trapped) {
                col = IM_COL32(180, 60, 180, 255);
            }

            ImVec2 pos;

            if (agent.currentEdge != -1) {
                const Edge* edge = findEdgeInState(state, agent.currentEdge);

                if (edge) {
                    ImVec2 fromPos = nodePos(edge->from);
                    ImVec2 toPos = nodePos(edge->to);

                    if (agent.edgeTargetNode == edge->from) {
                        std::swap(fromPos, toPos);
                    }

                    float dx = toPos.x - fromPos.x;
                    float dy = toPos.y - fromPos.y;
                    float len = std::sqrt(dx * dx + dy * dy);
                    if (len < 1e-5f) len = 1.0f;

                    float nx = -dy / len;
                    float ny =  dx / len;

                    float baseTime = static_cast<float>(edge->traversalTime);
                    if (baseTime <= 0.0f) baseTime = 1.0f;

                    float progress = 1.0f - static_cast<float>(agent.remainingEdgeTime / baseTime);
                    if (progress < 0.0f) progress = 0.0f;
                    if (progress > 1.0f) progress = 1.0f;

                    pos = ImVec2(
                        fromPos.x + (toPos.x - fromPos.x) * progress,
                        fromPos.y + (toPos.y - fromPos.y) * progress
                    );

                    float sideOffset = ((movingLane % 3) - 1) * 8.0f;
                    pos.x += nx * sideOffset;
                    pos.y += ny * sideOffset;
                    movingLane++;
                } else {
                    pos = nodePos(agent.currentNode);
                }
            } else {
                pos = nodePos(agent.currentNode);

                float offsetX = static_cast<float>((agent.id % 3) - 1) * 18.0f;
                float offsetY = static_cast<float>((agent.id / 3)) * 14.0f;
                pos.x += offsetX;
                pos.y += offsetY;
            }

            dl->AddCircleFilled(pos, 9.0f, col);
            dl->AddCircle(pos, 9.0f, IM_COL32(255, 255, 255, 255), 0, 1.6f);

            char aBuf[16];
            std::snprintf(aBuf, sizeof(aBuf), "A%d", agent.id);
            dl->AddText(ImVec2(pos.x + 10.0f, pos.y - 8.0f), IM_COL32_WHITE, aBuf);
        }

        // Legenda
        ImVec2 legend(origin.x + 15.0f, origin.y + 640.0f);
        dl->AddText(legend, IM_COL32_WHITE, "Legenda:");
        dl->AddText(ImVec2(legend.x, legend.y + 22.0f), IM_COL32(60, 140, 255, 255), "Plavo = izlaz");
        dl->AddText(ImVec2(legend.x, legend.y + 44.0f), IM_COL32(175, 120, 235, 255), "Ljubicastо = stepenice");
        dl->AddText(ImVec2(legend.x, legend.y + 66.0f), IM_COL32(255, 170, 0, 255), "Narancasto = agent u kretanju");
        dl->AddText(ImVec2(legend.x, legend.y + 88.0f), IM_COL32(255, 90, 90, 255), "Crveno = agent ceka");
        dl->AddText(ImVec2(legend.x, legend.y + 110.0f), IM_COL32(80, 220, 100, 255), "Zeleno = evakuiran");
        dl->AddText(ImVec2(legend.x, legend.y + 132.0f), IM_COL32_WHITE, "x/y = zauzece / kapacitet hodnika");

        ImGui::Dummy(ImVec2(980.0f, 790.0f));
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}