using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class Game : MonoBehaviour
{
    public GameObject chesspiece;
	public GameObject freezeSpellW;
	public GameObject freezeSpellB;
	public GameObject jumpSpellW;
	public GameObject jumpSpellB;

	Spells cm;
	//Stuff for Fen Creation
	Dictionary<string, string>FenCodes = new Dictionary<string, string>(32);

	int wTurn = 0;
	int	fTurn = 0;
	public string[] enPasPos = new string[1];

	//Can castle boolean values for Fen (King side, Queen side)
	public bool K = true;
	public bool Q = true;
	public bool k = true;
	public bool q = true;

	public string frozenPiece;
	public string jumpPiece;

	//Positions and team for each chesspiece
	private GameObject[,] positions = new GameObject[8, 8];
    public GameObject[] playerBlack = new GameObject[16];
	public GameObject[] playerWhite = new GameObject[16];

	public GameObject[] frozenBlack = new GameObject[9];           // Keeps track of the frozen pieces each turn.
	public GameObject[] frozenWhite = new GameObject[9];

	public GameObject[,] freezePlates = new GameObject[8, 8];
	public GameObject[,] jumpPlates = new GameObject[8, 8];

	Color WColor;
	Color BColor;

	private string currentPlayer = "white";
    private bool gameOver = false;

	public bool lastOneTurnW = false;
	public bool lastOneTurnB = false; //Im going to use this to keep track of stuff that needs to last a turn before reverting.

	//Keeps track of how many spells each player can use
	public int WFreezeSpells;
	public int BFreezeSpells;
	public int WJumpSpells;
	public int BJumpSpells;

	//Gonna keep track of this using integers for now, not sure still since you can use them after 3 turn cool down
	public int wCanUseSpell = 0;
	public int bCanUseSpell = 0;

	public int wCanUseSpellj = 0; //Jump Spells
	public int bCanUseSpellj = 0;
	void Start() // this is where everything will start when program runs
    {
		WFreezeSpells = 5;
		BFreezeSpells = 5;
		WJumpSpells = 2;
		BJumpSpells = 2;


		FenCodes.Add("black_rook", "r");
		FenCodes.Add("black_knight", "n");
		FenCodes.Add("black_bishop", "b");
		FenCodes.Add("black_pawn", "p");
		FenCodes.Add("black_queen", "q");
		FenCodes.Add("black_King", "k");

		FenCodes.Add("white_rook", "R");
		FenCodes.Add("white_knight", "N");
		FenCodes.Add("white_bishop", "B");
		FenCodes.Add("white_pawn", "P");
		FenCodes.Add("white_queen", "Q");
		FenCodes.Add("white_King", "K");

		//setting up Spell Counters
		GameObject.FindGameObjectWithTag("FreezeCount").GetComponent<Text>().enabled = true;
		GameObject.FindGameObjectWithTag("FreezeCount").GetComponent<Text>().color = Color.white;
		GameObject.FindGameObjectWithTag("FreezeCount").GetComponent<Text>().text = WFreezeSpells.ToString();

		GameObject.FindGameObjectWithTag("FreezeCountB").GetComponent<Text>().enabled = true;
		GameObject.FindGameObjectWithTag("FreezeCountB").GetComponent<Text>().color = Color.white;
		GameObject.FindGameObjectWithTag("FreezeCountB").GetComponent<Text>().text = BFreezeSpells.ToString();

		GameObject.FindGameObjectWithTag("JumpCount").GetComponent<Text>().enabled = true;
		GameObject.FindGameObjectWithTag("JumpCount").GetComponent<Text>().color = Color.white;
		GameObject.FindGameObjectWithTag("JumpCount").GetComponent<Text>().text = WJumpSpells.ToString();

		GameObject.FindGameObjectWithTag("JumpCountB").GetComponent<Text>().enabled = true;
		GameObject.FindGameObjectWithTag("JumpCountB").GetComponent<Text>().color = Color.white;
		GameObject.FindGameObjectWithTag("JumpCountB").GetComponent<Text>().text = BJumpSpells.ToString();



		GameObject.FindGameObjectWithTag("SpellsTitle").GetComponent<Text>().enabled = true;
		//Sets all the pieces in their initial spots in an array.
		playerWhite = new GameObject[]{ Create("white_rook",0,0), Create("white_knight",1,0),
			Create("white_bishop",2,0), Create("white_queen",3,0), Create("white_King",4,0),
			Create("white_bishop",5,0), Create("white_knight",6,0), Create("white_rook",7,0),
			Create("white_pawn",0,1), Create("white_pawn",1,1), Create("white_pawn",2,1),
			Create("white_pawn",3,1), Create("white_pawn",4,1), Create("white_pawn",5,1),
			Create("white_pawn",6,1), Create("white_pawn",7,1)};

		playerBlack = new GameObject[]{ Create("black_rook",0,7), Create("black_knight",1,7), 
			Create("black_bishop",2,7), Create("black_queen",3,7), Create("black_King",4,7), 
			Create("black_bishop",5,7), Create("black_knight",6,7), Create("black_rook",7,7), 
			Create("black_pawn",0,6), Create("black_pawn",1,6), Create("black_pawn",2,6), 
			Create("black_pawn",3,6), Create("black_pawn",4,6), Create("black_pawn",5,6), 
			Create("black_pawn",6,6), Create("black_pawn",7,6)};

		BColor = playerBlack[0].GetComponent<SpriteRenderer>().material.color;
		WColor = playerWhite[0].GetComponent<SpriteRenderer>().material.color;

		//Set all peices on the board.
		for ( int i = 0; i < playerBlack.Length; i++)
		{
			SetPosition(playerBlack[i]);
			SetPosition(playerWhite[i]);
		}

		//Sets up the Freeze and jump buttons
		GameObject FreezeSpellW = Instantiate(freezeSpellW, new Vector3(-2, -4, -1), Quaternion.identity);
		cm = FreezeSpellW.GetComponent<Spells>();
		cm.name = "whiteFreeze";
		cm.Activate();
		GameObject FreezeSpellB = Instantiate(freezeSpellB, new Vector3(-2, 4, -1), Quaternion.identity);
		cm = FreezeSpellB.GetComponent<Spells>();
		cm.name = "blackFreeze";
		cm.Activate();

		GameObject JumpSpellW = Instantiate(jumpSpellW, new Vector3(2, -4, -1), Quaternion.identity);
		cm = JumpSpellW.GetComponent<Spells>();
		cm.name = "whiteJump";
		cm.Activate();
		GameObject JumpSpellB = Instantiate(jumpSpellB, new Vector3(2, 4, -1), Quaternion.identity);
		cm = JumpSpellB.GetComponent<Spells>();
		cm.name = "blackJump";
		cm.Activate();
	}
	public GameObject Create(string name, int x, int y)
	{
		GameObject obj = Instantiate(chesspiece, new Vector3(0, 0, -1), Quaternion.identity);
		Chessman cm = obj.GetComponent<Chessman>();
		cm.name = name;
		cm.SetXBoard(x);
		cm.SetYBoard(y);
		cm.Activate();
		return obj;
	}
	public void SetPosition(GameObject obj)
	{
		Chessman cm = obj.GetComponent<Chessman>();

		positions[cm.GetXBoard(), cm.GetYBoard()] = obj;
	}
	public void SetPositionEmpty(int x, int y)
	{
		positions[x, y] = null;
	}
	public GameObject GetPosition(int x, int y)
	{
		return positions[x, y];
	}
	public bool PositionOnBoard(int x, int y)
	{
		if (x < 0 || y < 0 || x >= positions.GetLength(0) || y >= positions.GetLength(1)) return false;
		return true;
	}
	public string GetCurrentPlayer()
	{
		return currentPlayer;
	}
	public bool IsGameOver()
	{
		return gameOver; 
	}

	public void NextTurn() //Everything That needs to happen between turns goes here
	{
		
		//false meaning it doesnt need to last a turn before reverting.
		if (lastOneTurnB == false) //Checks to see if it needs to be skipped once for black player (Freeze spell)
		{
			FreezePlate.lastClickedPlateCoords.Set(-1, -1);


			for (int i = 0; i < frozenBlack.Length; i++)
			{
				if (frozenBlack[i] != null)
				{
					frozenBlack[i].GetComponent<Chessman>().canMove = true;
					frozenBlack[i].GetComponent<SpriteRenderer>().material.color = BColor;
					frozenBlack[i] = null;
				}
			}
		}

		if (lastOneTurnW == false)
		{
			FreezePlate.lastClickedPlateCoords.Set(-1, -1);
			for (int i = 0; i < frozenWhite.Length; i++)
			{
				if (frozenWhite[i] != null)
				{

					frozenWhite[i].GetComponent<Chessman>().canMove = true;
					frozenWhite[i].GetComponent<SpriteRenderer>().material.color = WColor;
					playerWhite[i] = null;
				}
			}
		}
		if (currentPlayer == "white")
		{
			//These will decrese the spell cooldown after each turn
			if(wCanUseSpell != 0)
			{
				wCanUseSpell -= 1;
			}
			if (wCanUseSpellj != 0)
			{
				wCanUseSpellj -= 1;
			}

			fTurn += 1;
			currentPlayer = "black";
			lastOneTurnW = false;
		}
		else
		{
			if (bCanUseSpell != 0)
			{
				bCanUseSpell -= 1;
			}
			if (bCanUseSpellj != 0)
			{
				bCanUseSpellj -= 1;
			}                                               //Keeps track of half turns
			fTurn += 1;
			currentPlayer = "white";
			lastOneTurnB = false;
		}

		for (int i = 0; i < playerWhite.Length; i++)						//Sinplest way to take care of making sure pieces dont stay jumpable
		{
			if (playerWhite[i] != null && playerWhite[i].GetComponent<Chessman>().jumpable)
			{
				playerWhite[i].GetComponent<Chessman>().jumpable = false;
				playerWhite[i].GetComponent<SpriteRenderer>().material.color = WColor;
			}
		}
		for (int i = 0; i < playerBlack.Length; i++)
		{
			if (playerBlack[i] != null && playerBlack[i].GetComponent<Chessman>().jumpable)
			{
				playerBlack[i].GetComponent<Chessman>().jumpable = false;
				playerBlack[i].GetComponent<SpriteRenderer>().material.color = BColor;
			}
		}

		GenerateFen();
	}
	public void GenerateFen()
	{
		int EmptyPos = 0;
		string Fen = "";
		for (int i = 7; i >= 0; i--)
		{
			for (int j = 0; j < 8; j++)
			{

				if(EmptyPos == 8)
				{
					Fen += EmptyPos.ToString();
					EmptyPos = 0;
				}
				if (GetPosition(j, i) != null) 
				{
					if (EmptyPos != 0)
					{
						Fen += EmptyPos.ToString();
						EmptyPos = 0;
					}
					Fen = Fen + FenCodes[GetPosition(j, i).name.ToString()];
					
				}
				else
				{
					EmptyPos++;
				}
			}
			if(EmptyPos != 0)
				Fen += EmptyPos.ToString();
			Fen += '/';
			EmptyPos = 0;
		}


		Fen += " "+currentPlayer[0] + " ";
		if (K)
			Fen += "K";
		if (Q)
			Fen += "Q";
		if (k)
			Fen += "k";
		if (q)
			Fen += "q";
		if (!K && !Q && !k && !q)
			Fen += "-";

		if (enPasPos[0] == "")
			Fen += " -";
		else
			Fen += " " + enPasPos[0];

		Fen += " " + wTurn.ToString() + fTurn.ToString();

		if (frozenPiece == "")
			Fen += " -";
		else
		{
			Fen += " " + frozenPiece;
			frozenPiece = "";
		}

		if (jumpPiece == "")
			Fen += " -";
		else
		{
			Fen += " " + jumpPiece;
			jumpPiece = "";
		}
		Fen += " " + WFreezeSpells.ToString() + WJumpSpells.ToString() + BFreezeSpells.ToString() + BJumpSpells.ToString();
		Fen += " " + wCanUseSpell.ToString() + wCanUseSpellj.ToString() + bCanUseSpell.ToString() + bCanUseSpellj.ToString();

		print(Fen);
	}

	public void Update()
	{
		if (gameOver == true && Input.GetMouseButtonDown(0))
		{
			gameOver = false;

			SceneManager.LoadScene("Game");
		}
		GameObject.FindGameObjectWithTag("FreezeCount").GetComponent<Text>().text = WFreezeSpells.ToString();
		GameObject.FindGameObjectWithTag("FreezeCountB").GetComponent<Text>().text = BFreezeSpells.ToString();
		GameObject.FindGameObjectWithTag("JumpCount").GetComponent<Text>().text = WJumpSpells.ToString();
		GameObject.FindGameObjectWithTag("JumpCountB").GetComponent<Text>().text = BJumpSpells.ToString();

	}
	public void Winner(string playerWinner)
	{
		gameOver = true;

		GameObject.FindGameObjectWithTag("WinnerText").GetComponent<Text>().enabled = true;
		GameObject.FindGameObjectWithTag("WinnerText").GetComponent<Text>().text = playerWinner + " is the winner!";

		GameObject.FindGameObjectWithTag("RestartText").GetComponent<Text>().enabled = true;
	}

}
