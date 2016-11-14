class CrushModel {
  int gameid; //unique identifier
  Array2D extensionColor; //the extension grid
  Array2D boardInitialState; //how many times each square must fire
  int movesAllowed; //max moves allowed
  int colors; //max # of distinct colors in extension board

  Array2D boardCandies; //current candies on the board
  Array2D boardState; //how many times each square must fire
  int movesMade; //how many moves have been made
  int currentScore; //total amount that state has decreased
  int extensionOffset[];

  public:
    CrushModel();

};

CrushModel::CrushModel(int gameid, Array2D extensionColor,
		       Array2D boardInitialState, int movesAllowed,
		       int colors, Array2D boardCandies, Array2D boardState,
		       int movesMade, int currentScore, 
		       int extensionOffset[]){

  this.gameid = gameid;
  this.extensionColor = extensionColor;
  this.boardInitialState = boardInitialState;
  this.movesAllowed = movesAllowed;
  this.colors = boardCandies;
  this.boardState = boardState;
  this.movesMade = movesMade;
  this.currentScore = currentScore;
  this.extensionOffset = extensionOffset;
}

void deserializeGameInstance(char* location){
  json_error_t error;
  json_t* json = json_load_file(location, 0, &error);
  if (!json){
    return NULL;
  }
  //game def
  json_t* gameDef = json_object_get(json, "gamedef");

  //game id
  jsont_t* jGameid = json_object_get(gameDef, "gameid");
  int gameid = json_integer_value(jGameid);

  //extensioncolor
  json_t* jExtensionColor = json_object_get(gameDef, "extensioncolor");
  json_t* jExtensionRows = json_object_get(jExtensionColor, "rows");
  json_t* jExtensionColumns = json_object_get(jExtensionColor, "columns");
  int extensionRows = json_integer_value(jExtensionRows);
  int extensionColumns = json_integer_value(jExtensionColumns);
  Array2D extensionColor = allocateArray2D(extensionRows, extensionColumns);
  json_t* extensionData = json_object_get(jExtensionColor, "data");
  int extensionSize = json_array_size(extensionData);
  int* extensionElements = (int*) malloc(sizeof(int) * extensionSize));
  for (int i = 0; i < extensionSize; i++){
    extensionElements[i] = json_integer_value(json_array_get(extensionData, i));
    setArray2D(extensionColor, i % extensionColumns, i / extensionColumns);
  }
  printArray(extensionColor);
}

int main(int argc, char** argv){
  deserializeGameInstance(argv[1]);
}
