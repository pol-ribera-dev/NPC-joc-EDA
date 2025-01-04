#include "Player.hh"
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Yasuo_0_8_0


struct PLAYER_NAME : public Player {
	
  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }
	
  /**
   * Types and attributes for your player can be defined here.
   */
	const vector<Dir> dirs = {Up,Down,Left,Right};

  /**
   * Play method, invoked once per each round.
   */
  void moure_random(int id){
    Dir aleatori = dirs[random(0,dirs.size()-1)]; 
	  Pos posicio_random = unit(id).pos + aleatori;
	  if (pos_ok(posicio_random) and cell(posicio_random.i,posicio_random.j).type != Waste) move(id,aleatori);
    else moure_random(id);
  }

  void BFS(vector<Pos>& mirar, int dist[60][60]){ 
    Pos casella;
    queue<Pos> Mirar_pos2; 
    for (auto comida : mirar){
      while(!Mirar_pos2.empty()) Mirar_pos2.pop();
      bool player_found = false; //al trobar un del teu equip parar
      bool jo = false;
      Mirar_pos2.push(comida);
      while(!Mirar_pos2.empty() and !jo){ 
        casella = Mirar_pos2.front();
        for (auto d : dirs) { //per totes les direccions
          Pos pos_mirar = casella + d;
          if (pos_ok(pos_mirar) and !player_found){ //quan hi ha dos de diferents equips i si anar
            if (cell(pos_mirar).type == Street and dist[pos_mirar.i][pos_mirar.j] > (dist[casella.i][casella.j] + 1) and (cell(pos_mirar).id == -1 or unit(cell(pos_mirar).id).type != Dead)){ //==100000 enlloc de < (dist[casella.i][casella.j] + 1)
              Mirar_pos2.push(pos_mirar);
              dist[pos_mirar.i][pos_mirar.j] = dist[casella.i][casella.j] + 1;
              if(cell(pos_mirar).id != -1){
                if(unit(cell(pos_mirar).id).type == Alive and (strength(me())*alive_units(me()).size() < 500 or alive_units(me()).size() > 15)){ 
                  player_found = true;
                  if(unit(cell(pos_mirar).id).player == me()) jo = true;
                } else if(unit(cell(pos_mirar).id).type == Alive and unit(cell(pos_mirar).id).player == me()){
                  player_found = true;
                  jo = true;
                }
              }
            }
          }
        }
        Mirar_pos2.pop();
      }
    }
  }
  
  virtual void play () {
    vector<pair<int, int>> ranking;
    for(int i = 0; i <=3; i++){
      if(player_ok(i)){
       ranking.push_back({score(i),i});
      }
    }
    sort(ranking.begin(), ranking.end(), greater<pair<int, int>>());
    ranking.push_back({0,-1});
    double st = status(me());
    if (st >= 0.9) return;
    
    int y_food = 0;
    int x_food = 0;
    bool food_in_cell;
    int caselles[60][60];
    vector<Pos> Mirar_pos;


    while(y_food != 60){  //buscar tot el menjar fent un recorregut del mapa
      food_in_cell = cell(x_food,y_food).food;
      if (food_in_cell){
        Mirar_pos.push_back({x_food, y_food});
        caselles[x_food][y_food]= 0;

      }else{
        caselles[x_food][y_food]= 100000;
      }
      x_food++;
      if (x_food == 60){
        x_food = 0;
        y_food++;
      }
    }
    int y_test = 0;
    int x_test = 0; 
   
    BFS(Mirar_pos, caselles); //fer el BFS desde el menjar fins a trobar un jugadors (per acabar)
    vector<int> alive = alive_units(me());
    vector<bool> tirats(91,false);
    for (int id : alive) {//matar tots el jugadors que estan al costat 
      for (auto d : dirs) {
        Pos new_pos = unit(id).pos + d;
        if (pos_ok(new_pos)) {
          int id_in_cell = cell(new_pos.i,new_pos.j).id;
          if (id_in_cell != -1 and unit(id_in_cell).type == Alive and unit(id_in_cell).player != me()) {move(id,d); tirats[id]=true; break;}//comparació de força
        }
      }  
    }

    for (int id : alive) {//matar tots els zombies del costat
      if(tirats[id]==false){
        for (auto itr = dirs.begin(); itr != dirs.end(); ++itr) {
          Dir d= *itr;
          Pos new_pos = unit(id).pos + d;
          if (pos_ok(new_pos)) {
            int id_in_cell = cell(new_pos.i,new_pos.j).id;
            if (id_in_cell != -1 and unit(id_in_cell).type == Zombie) {move(id,d); tirats[id]=true; break;}
          }
        }
      }
    }
    vector<pair<int, Dir>> vec_predicts;
    for (int id : alive) { 
      if(tirats[id]==false){
        //mirar quines posicions els zombies em poden matar si hi vaig
        vector<Dir> no_evitar = {}; 
        bool foundz = false;
        if(unit(id).rounds_for_zombie == -1){
          for (auto itr = dirs.begin(); itr != dirs.end(); ++itr) {
            Dir d= *itr;
            Pos new_pos = unit(id).pos + d;
            if (pos_ok(new_pos)) {
              foundz = false;
              for (auto d1 : dirs) {
                Pos new_new_pos = new_pos + d1;
                if (pos_ok(new_new_pos)) {
                  if (new_new_pos != new_pos){
                    int id_in_cell2 = cell(new_new_pos.i,new_new_pos.j).id;
                    if (id_in_cell2 != -1 and unit(id_in_cell2).type == Zombie ){
                      foundz = true;
                      break;
                    }
                  }
                  if(d1 == d){
                    for (auto d2 : dirs) {
                      if(d2 != d1){
                        Pos new_new_new_pos = new_new_pos + d2;
                        if (pos_ok(new_new_new_pos)) {
                          if (new_new_pos != new_new_new_pos){
                          int id_in_cell3 = cell(new_new_new_pos.i,new_new_new_pos.j).id;
                            if (id_in_cell3 != -1 and unit(id_in_cell3).type == Zombie){
                              foundz = true;
                              break;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
              if (foundz == false){
                no_evitar.push_back(d);
              }
            }
          }
        }else{
          no_evitar = dirs;
          for (auto d : dirs) {
            Pos new_pos = unit(id).pos + d;
            if(pos_ok(new_pos) and cell(new_pos).type != Waste and cell(new_pos).id != -1 and unit(cell(new_pos).id).player == me()){ 
              move(id,d);
              tirats[id]=true;
              break;
            }
          }
        }

        if(round() == 150 or alive_units(me()).size() > 35){ //desesperat
          no_evitar = dirs;
        }

        //si li pertany un menjar anar a buscar-lo 
        int minim = 1000000;
        Dir best_d = Up;
        if(no_evitar.size() == 0) no_evitar = dirs; 
        for (auto d : no_evitar) {
          Pos new_pos = unit(id).pos + d;
          if (pos_ok(new_pos) and cell(new_pos).type != Waste and (cell(new_pos).id == -1 or unit(cell(new_pos).id).type != Dead)){
            if(minim >= caselles[new_pos.i][new_pos.j]){
              minim = caselles[new_pos.i][new_pos.j];
              best_d = d;
            }
          }
        }

        //moviment base: anar a buscar caselles que no son teves, fer que primer vagis a les dels altres
        if(minim < ((200-round())-25)){
          move(id,best_d);
          tirats[id]=true;
        }else{ //si hi ha un jugador al que pots fer predict 
          for (auto d : dirs) {
            Pos new_pos = unit(id).pos + d;
            if(pos_ok(new_pos) and cell(new_pos).type != Waste and cell(new_pos).id != -1 and unit(cell(new_pos).id).rounds_for_zombie > -1){ 
              if(unit(id).rounds_for_zombie == -1 or unit(id).rounds_for_zombie > unit(cell(new_pos).id).rounds_for_zombie ){
                move(id,d);
                tirats[id]=true;
                break;
              }
            }
          }

          if(tirats[id]==false){
            int millor_forza = 10000000; //fer predict al que te menys força 
            pair<int, Dir> mil_pre;
            for (auto d : no_evitar) {
              Pos new_pos = unit(id).pos + d;
              if (pos_ok(new_pos) and cell(new_pos).type != Waste and cell(new_pos).id == -1){
                for (auto d1 : dirs) {
                  Pos new_new_pos = new_pos + d1;
                  if (pos_ok(new_new_pos)) {
                    if (new_new_pos != new_pos){
                      int id_cell = cell(new_new_pos.i,new_new_pos.j).id;
                      if (id_cell != -1 and unit(id_cell).type == Alive and unit(id_cell).player != me()){
                        if (5*strength(me()) >  2*strength(unit(id_cell).player)){
                          tirats[id]=true;
                          if(strength(unit(id_cell).player) < millor_forza){
                            mil_pre = {id,d};
                            millor_forza = strength(unit(id_cell).player);
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            if (tirats[id]==true){
              vec_predicts.push_back(mil_pre);
            }else{
              for(pair<int, int> i : ranking){
                int j = i.second;
                if(tirats[id]==false and j != me()){
                  for (auto d : no_evitar) {
                    Pos new_pos = unit(id).pos + d;
                    if(pos_ok(new_pos) and cell(new_pos).type != Waste and cell(new_pos).owner == j){ 
                      move(id,d);
                      tirats[id]=true;
                      break;
                    }
                  }
                }
              }
              if(tirats[id]==false){
                pair<Pos, Dir> casella;
                queue<pair<Pos, Dir>> bfs_caselles;
                bool trobat = false; 
                for (auto d : no_evitar) { 
                  Pos nova_posicio = unit(id).pos + d;
                  if (pos_ok(nova_posicio)){
                    if (cell(nova_posicio).type == Street){
                      bfs_caselles.push({nova_posicio, d});
                    }
                  }
                }
                vector<bool> vis(3600, false);
                while(!bfs_caselles.empty() and !trobat){ 
                  casella = bfs_caselles.front();
                  for (auto d : dirs) {
                    Pos pos_mirar = casella.first + d;
                    if (pos_ok(pos_mirar)){
                      if (cell(pos_mirar).type == Street){ 
                        if(cell(pos_mirar).owner != me()){
                          move(id,casella.second);
                          tirats[id]=true; 
                          trobat = true;
                          break;
                        }else{
                          if (!vis[pos_mirar.i*60+pos_mirar.j]){
                            vis[pos_mirar.i*60+pos_mirar.j] = true;
                            bfs_caselles.push({pos_mirar, casella.second});
                          }
                        }
                      }
                    }
                  }
                  bfs_caselles.pop();
                }
                if(tirats[id]==false){
                  moure_random(id);
                }
              }//}
            }
          }
        }
      }
    } 
    
    for (pair<int, Dir> id : vec_predicts) { 
      move(id.first, id.second);
    }
	}
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);