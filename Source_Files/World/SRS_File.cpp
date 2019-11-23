	std::unique_ptr<std::unique_ptr<SDL_Point[]>[]> I_OffSet;
	I_OffSet = std::make_unique<std::unique_ptr<SDL_Point[]>[]>(4);
	for(int i=0; i<4; i++) {
		I_OffSet[i] = std::make_unique<SDL_Point[]>(5);
	}
	I_OffSet[0][0].x = 0;
	I_OffSet[0][0].y = 0; //IS THERE NOT A BETTER WAY?

	I_OffSet[0][1].x = -1;
	I_OffSet[0][1].y = 0;

	I_OffSet[0][2].x = 2;
	I_OffSet[0][2].y = 0;

	I_OffSet[0][3].x = -1;
	I_OffSet[0][3].y = 0;

	I_OffSet[0][4].x = 2;
	I_OffSet[0][4].y = 0;


	I_OffSet[1][0].x = -1;
	I_OffSet[1][0].y = 0;

	I_OffSet[1][1].x = 0;
	I_OffSet[1][1].y = 0;

	I_OffSet[1][2].x = 0;
	I_OffSet[1][2].y = 0;

	I_OffSet[1][3].x = 0;
	I_OffSet[1][3].y = 1;

	I_OffSet[1][4].x = 0;
	I_OffSet[1][4].y = -2;

	
	I_OffSet[2][0].x = -1;
	I_OffSet[2][0].y = 1;

	I_OffSet[2][1].x = 1;
	I_OffSet[2][1].y = 1;

	I_OffSet[2][2].x = -2;
	I_OffSet[2][2].y = 1;

	I_OffSet[2][3].x = 1;
	I_OffSet[2][3].y = 0;

	I_OffSet[2][4].x = -2;
	I_OffSet[2][4].y = 0;


	I_OffSet[3][0].x = 0;
	I_OffSet[3][0].y = 1;

	I_OffSet[3][1].x = 0;
	I_OffSet[3][1].y = 1;

	I_OffSet[3][2].x = 0;
	I_OffSet[3][2].y = 1;

	I_OffSet[3][3].x = 0;
	I_OffSet[3][3].y = -1;

	I_OffSet[3][4].x = 0;
	I_OffSet[3][4].y = 2;




	std::unique_ptr<std::unique_ptr<SDL_Point[]>[]> O_OffSet;
	O_OffSet = std::make_unique<std::unique_ptr<SDL_Point[]>[]>(4);
	for(int i=0; i<4; i++) {
		O_OffSet[i] = std::make_unique<SDL_Point[]>(5);
	}
	O_OffSet[0][0].x = 0;
	O_OffSet[0][0].y = 0; //AAAAAAAAAAAAAAAAAAAA

	O_OffSet[0][1].x = 0; //so much precious space, wasted
	O_OffSet[0][1].y = 0;

	O_OffSet[0][2].x = 0;
	O_OffSet[0][2].y = 0;

	O_OffSet[0][3].x = 0;
	O_OffSet[0][3].y = 0;

	O_OffSet[0][4].x = 0;
	O_OffSet[0][4].y = 0;


	O_OffSet[1][0].x = 0;
	O_OffSet[1][0].y = -1;

	O_OffSet[1][1].x = 0;
	O_OffSet[1][1].y = 0;

	O_OffSet[1][2].x = 0;
	O_OffSet[1][2].y = -2;

	O_OffSet[1][3].x = 0;
	O_OffSet[1][3].y = 0;

	O_OffSet[1][4].x = 0;
	O_OffSet[1][4].y = 0;

	
	O_OffSet[2][0].x = -1;
	O_OffSet[2][0].y = -1;

	O_OffSet[2][1].x = 0;
	O_OffSet[2][1].y = 0;

	O_OffSet[2][2].x = -2;
	O_OffSet[2][2].y = -2;

	O_OffSet[2][3].x = 0;
	O_OffSet[2][3].y = 0;

	O_OffSet[2][4].x = 0;
	O_OffSet[2][4].y = 0;


	O_OffSet[3][0].x = -1;
	O_OffSet[3][0].y = 0;

	O_OffSet[3][1].x = 0;
	O_OffSet[3][1].y = 0;

	O_OffSet[3][2].x = -2;
	O_OffSet[3][2].y = 0;

	O_OffSet[3][3].x = 0;
	O_OffSet[3][3].y = 0;

	O_OffSet[3][4].x = 0;
	O_OffSet[3][4].y = 0;




	std::unique_ptr<std::unique_ptr<SDL_Point[]>[]> Other_OffSet;
	Other_OffSet = std::make_unique<std::unique_ptr<SDL_Point[]>[]>(4);
	for(int i=0; i<4; i++) {
		Other_OffSet[i] = std::make_unique<SDL_Point[]>(5);
	}
	Other_OffSet[0][0].x = 0;
	Other_OffSet[0][0].y = 0; //KILL MEEEEEE

	Other_OffSet[0][1].x = 0; //SOMEONE, SEND HELP
	Other_OffSet[0][1].y = 0; //PLEASE!

	Other_OffSet[0][2].x = 0;
	Other_OffSet[0][2].y = 0;

	Other_OffSet[0][3].x = 0;
	Other_OffSet[0][3].y = 0;

	Other_OffSet[0][4].x = 0;
	Other_OffSet[0][4].y = 0;


	Other_OffSet[1][0].x = 0;
	Other_OffSet[1][0].y = 0;

	Other_OffSet[1][1].x = 1;
	Other_OffSet[1][1].y = 0;

	Other_OffSet[1][2].x = 1;
	Other_OffSet[1][2].y = -1;

	Other_OffSet[1][3].x = 0;
	Other_OffSet[1][3].y = 2;

	Other_OffSet[1][4].x = 1;
	Other_OffSet[1][4].y = 2;

	
	Other_OffSet[2][0].x = 0;
	Other_OffSet[2][0].y = 0;

	Other_OffSet[2][1].x = 0;
	Other_OffSet[2][1].y = 0;

	Other_OffSet[2][2].x = 0;
	Other_OffSet[2][2].y = 0;

	Other_OffSet[2][3].x = 0;
	Other_OffSet[2][3].y = 0;

	Other_OffSet[2][4].x = 0;
	Other_OffSet[2][4].y = 0;


	Other_OffSet[3][0].x = 0;
	Other_OffSet[3][0].y = 0;

	Other_OffSet[3][1].x = -1;
	Other_OffSet[3][1].y = 0;

	Other_OffSet[3][2].x = -1;
	Other_OffSet[3][2].y = -1;

	Other_OffSet[3][3].x = 0;
	Other_OffSet[3][3].y = 2;

	Other_OffSet[3][4].x = -1;
	Other_OffSet[3][4].y = 2;

	//finally done, only 221 lines of code wasted
	//funck you SRS