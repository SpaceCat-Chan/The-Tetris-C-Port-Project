	SDL_Point* I_OffSet[Rotations::TOTAL_ROTATIONS];
	//SDL_Point I_OffSet[Rotations::ORIGINAL][5];
	I_OffSet[Rotations::ORIGINAL][0].x = 0;
	I_OffSet[Rotations::ORIGINAL][0].y = 0; //IS THERE NOT A BETTER WAY?

	I_OffSet[Rotations::ORIGINAL][1].x = -1;
	I_OffSet[Rotations::ORIGINAL][1].y = 0;

	I_OffSet[Rotations::ORIGINAL][2].x = 2;
	I_OffSet[Rotations::ORIGINAL][2].y = 0;

	I_OffSet[Rotations::ORIGINAL][3].x = -1;
	I_OffSet[Rotations::ORIGINAL][3].y = 0;

	I_OffSet[Rotations::ORIGINAL][4].x = 2;
	I_OffSet[Rotations::ORIGINAL][4].y = 0;


	I_OffSet[Rotations::RIGHT][0].x = -1;
	I_OffSet[Rotations::RIGHT][0].y = 0;

	I_OffSet[Rotations::RIGHT][1].x = 0;
	I_OffSet[Rotations::RIGHT][1].y = 0;

	I_OffSet[Rotations::RIGHT][2].x = 0;
	I_OffSet[Rotations::RIGHT][2].y = 0;

	I_OffSet[Rotations::RIGHT][3].x = 0;
	I_OffSet[Rotations::RIGHT][3].y = 1;

	I_OffSet[Rotations::RIGHT][4].x = 0;
	I_OffSet[Rotations::RIGHT][4].y = -2;

	
	I_OffSet[Rotations::DOUBLE][0].x = -1;
	I_OffSet[Rotations::DOUBLE][0].y = 1;

	I_OffSet[Rotations::DOUBLE][1].x = 1;
	I_OffSet[Rotations::DOUBLE][1].y = 1;

	I_OffSet[Rotations::DOUBLE][2].x = -2;
	I_OffSet[Rotations::DOUBLE][2].y = 1;

	I_OffSet[Rotations::DOUBLE][3].x = 1;
	I_OffSet[Rotations::DOUBLE][3].y = 0;

	I_OffSet[Rotations::DOUBLE][4].x = -2;
	I_OffSet[Rotations::DOUBLE][4].y = 0;


	I_OffSet[Rotations::LEFT][0].x = 0;
	I_OffSet[Rotations::LEFT][0].y = 1;

	I_OffSet[Rotations::LEFT][1].x = 0;
	I_OffSet[Rotations::LEFT][1].y = 1;

	I_OffSet[Rotations::LEFT][2].x = 0;
	I_OffSet[Rotations::LEFT][2].y = 1;

	I_OffSet[Rotations::LEFT][3].x = 0;
	I_OffSet[Rotations::LEFT][3].y = -1;

	I_OffSet[Rotations::LEFT][4].x = 0;
	I_OffSet[Rotations::LEFT][4].y = 2;




	SDL_Point* O_OffSet[Rotations::TOTAL_ROTATIONS];
	//SDL_Point O_OffSet[Rotations::ORIGINAL][5];
	O_OffSet[Rotations::ORIGINAL][0].x = 0;
	O_OffSet[Rotations::ORIGINAL][0].y = 0; //AAAAAAAAAAAAAAAAAAAA

	O_OffSet[Rotations::ORIGINAL][1].x = 0; //so much precious space, wasted
	O_OffSet[Rotations::ORIGINAL][1].y = 0;

	O_OffSet[Rotations::ORIGINAL][2].x = 0;
	O_OffSet[Rotations::ORIGINAL][2].y = 0;

	O_OffSet[Rotations::ORIGINAL][3].x = 0;
	O_OffSet[Rotations::ORIGINAL][3].y = 0;

	O_OffSet[Rotations::ORIGINAL][4].x = 0;
	O_OffSet[Rotations::ORIGINAL][4].y = 0;


	O_OffSet[Rotations::RIGHT][0].x = 0;
	O_OffSet[Rotations::RIGHT][0].y = -1;

	O_OffSet[Rotations::RIGHT][1].x = 0;
	O_OffSet[Rotations::RIGHT][1].y = 0;

	O_OffSet[Rotations::RIGHT][2].x = 0;
	O_OffSet[Rotations::RIGHT][2].y = 0;

	O_OffSet[Rotations::RIGHT][3].x = 0;
	O_OffSet[Rotations::RIGHT][3].y = 0;

	O_OffSet[Rotations::RIGHT][4].x = 0;
	O_OffSet[Rotations::RIGHT][4].y = 0;

	
	O_OffSet[Rotations::DOUBLE][0].x = -1;
	O_OffSet[Rotations::DOUBLE][0].y = -1;

	O_OffSet[Rotations::DOUBLE][1].x = 0;
	O_OffSet[Rotations::DOUBLE][1].y = 0;

	O_OffSet[Rotations::DOUBLE][2].x = 0;
	O_OffSet[Rotations::DOUBLE][2].y = 0;

	O_OffSet[Rotations::DOUBLE][3].x = 0;
	O_OffSet[Rotations::DOUBLE][3].y = 0;

	O_OffSet[Rotations::DOUBLE][4].x = 0;
	O_OffSet[Rotations::DOUBLE][4].y = 0;


	O_OffSet[Rotations::LEFT][0].x = 0;
	O_OffSet[Rotations::LEFT][0].y = -1;

	O_OffSet[Rotations::LEFT][1].x = 0;
	O_OffSet[Rotations::LEFT][1].y = 0;

	O_OffSet[Rotations::LEFT][2].x = 0;
	O_OffSet[Rotations::LEFT][2].y = 0;

	O_OffSet[Rotations::LEFT][3].x = 0;
	O_OffSet[Rotations::LEFT][3].y = 0;

	O_OffSet[Rotations::LEFT][4].x = 0;
	O_OffSet[Rotations::LEFT][4].y = 0;




	SDL_Point* Other_OffSet[Rotations::TOTAL_ROTATIONS];
	//SDL_Point Other_OffSet[Rotations::ORIGINAL][5];
	Other_OffSet[Rotations::ORIGINAL][0].x = 0;
	Other_OffSet[Rotations::ORIGINAL][0].y = 0; //KILL MEEEEEE

	Other_OffSet[Rotations::ORIGINAL][1].x = 0; //SOMEONE, SEND HELP
	Other_OffSet[Rotations::ORIGINAL][1].y = 0; //PLEASE!

	Other_OffSet[Rotations::ORIGINAL][2].x = 0;
	Other_OffSet[Rotations::ORIGINAL][2].y = 0;

	Other_OffSet[Rotations::ORIGINAL][3].x = 0;
	Other_OffSet[Rotations::ORIGINAL][3].y = 0;

	Other_OffSet[Rotations::ORIGINAL][4].x = 0;
	Other_OffSet[Rotations::ORIGINAL][4].y = 0;


	Other_OffSet[Rotations::RIGHT][0].x = 0;
	Other_OffSet[Rotations::RIGHT][0].y = 0;

	Other_OffSet[Rotations::RIGHT][1].x = 1;
	Other_OffSet[Rotations::RIGHT][1].y = 0;

	Other_OffSet[Rotations::RIGHT][2].x = 1;
	Other_OffSet[Rotations::RIGHT][2].y = -1;

	Other_OffSet[Rotations::RIGHT][3].x = 0;
	Other_OffSet[Rotations::RIGHT][3].y = 2;

	Other_OffSet[Rotations::RIGHT][4].x = 1;
	Other_OffSet[Rotations::RIGHT][4].y = 2;

	
	Other_OffSet[Rotations::DOUBLE][0].x = 0;
	Other_OffSet[Rotations::DOUBLE][0].y = 0;

	Other_OffSet[Rotations::DOUBLE][1].x = 0;
	Other_OffSet[Rotations::DOUBLE][1].y = 0;

	Other_OffSet[Rotations::DOUBLE][2].x = 0;
	Other_OffSet[Rotations::DOUBLE][2].y = 0;

	Other_OffSet[Rotations::DOUBLE][3].x = 0;
	Other_OffSet[Rotations::DOUBLE][3].y = 0;

	Other_OffSet[Rotations::DOUBLE][4].x = 0;
	Other_OffSet[Rotations::DOUBLE][4].y = 0;


	Other_OffSet[Rotations::LEFT][0].x = 0;
	Other_OffSet[Rotations::LEFT][0].y = 0;

	Other_OffSet[Rotations::LEFT][1].x = 0;
	Other_OffSet[Rotations::LEFT][1].y = -1;

	Other_OffSet[Rotations::LEFT][2].x = -1;
	Other_OffSet[Rotations::LEFT][2].y = -1;

	Other_OffSet[Rotations::LEFT][3].x = 0;
	Other_OffSet[Rotations::LEFT][3].y = 2;

	Other_OffSet[Rotations::LEFT][4].x = -1;
	Other_OffSet[Rotations::LEFT][4].y = 2;

	//finally done, only 221 lines of code wasted
	//funck you SRS