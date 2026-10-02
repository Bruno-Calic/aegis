#include <iostream>
#include "aegis/Scenario.hpp"
#include "aegis/simulation/SimulationEngine.hpp"
#include "aegis/pathfinding/PathFinder.hpp"
#include <iomanip>

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
    
    //Scenario agenti
    scenario.agents = {a1};

    //pokretanje simulacije
    SimulationEngine engine;
    engine.initialize(scenario);
    
    std::cout << "=== Aegis Simulacija ===\n";
    std::cout << "Scenarij: " << scenario.name << "\n";
    std::cout << "========================\n\n";

    //simulacija i obrada
    engine.step(0.25);

    //OBRADA//

    if(engine.isFinished()){
        std::cout << "Simulacija je završila.\n";
        std::cout << "Ukupno vrijeme: " << engine.state().currentTime << " s\n";
        std::cout << "Evakuirano: " << engine.state().evacuatedCount << "\n";
        std::cout << "Zarobljeno: " << engine.state().trappedCount << "\n";
        std::cout << "Ukupno koraka: " << engine.state().currentTime / 0.25 << "\n";
    } else {
        std::cout << "Simulacija još uvijek traje.\n";
    }

    return 0;
}
