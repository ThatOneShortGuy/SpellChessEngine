using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class Chessman : MonoBehaviour
{
	//References
	public GameObject controller;
	public GameObject movePlate;
	public bool isEnPassantable = false;
	public bool hasMoved = false;

	//Positions
	private int xBoard = -1; //-1 means its not on the board yet.
	private int yBoard = -1;

	// variable to keep track of "black" or "white player"
	private string player;

	//References for all the sprites or pieces. 
	public Sprite black_King, black_queen, black_knight, black_bishop, black_rook, black_pawn;
	public Sprite white_King, white_queen, white_knight, white_bishop, white_rook, white_pawn;

	public void Activate()
	{
		controller = GameObject.FindGameObjectWithTag("GameController");

		//takes the instantiated location and adjusts the transform
		SetCoords();

		switch (this.name)
		{
			case "black_King": 
				this.GetComponent<SpriteRenderer>().sprite = black_King; player = "black";
				hasMoved = false;
				break;
			case "black_queen": this.GetComponent<SpriteRenderer>().sprite = black_queen; player = "black"; break;
			case "black_knight": this.GetComponent<SpriteRenderer>().sprite = black_knight; player = "black"; break;
			case "black_bishop": this.GetComponent<SpriteRenderer>().sprite = black_bishop; player = "black"; break;
			case "black_rook": this.GetComponent<SpriteRenderer>().sprite = black_rook; player = "black";
				hasMoved = false;
				break;
			case "black_pawn": 
				this.GetComponent<SpriteRenderer>().sprite = black_pawn; player = "black";
				isEnPassantable = false;
				break;

			case "white_King": this.GetComponent<SpriteRenderer>().sprite = white_King; player = "white";
				hasMoved = false;
				break;
			case "white_queen": this.GetComponent<SpriteRenderer>().sprite = white_queen; player = "white"; break;
			case "white_knight": this.GetComponent<SpriteRenderer>().sprite = white_knight; player = "white"; break;
			case "white_bishop": this.GetComponent<SpriteRenderer>().sprite = white_bishop; player = "white"; break;
			case "white_rook": this.GetComponent<SpriteRenderer>().sprite = white_rook; player = "white";
				hasMoved = false;
				break;
			case "white_pawn":
				this.GetComponent<SpriteRenderer>().sprite = white_pawn; player = "white";
				isEnPassantable = false;
				break;
		}
	}

	public void SetCoords()
	{
		float x = xBoard;
		float y = yBoard;

		x *= 0.66f;
		y *= 0.66f;

		x += -2.3f;
		y += -2.3f;

		this.transform.position = new Vector3(x, y, -1.0f);

	}
	public int GetXBoard()
	{
		return xBoard;
	}
	public int GetYBoard()
	{
		return yBoard;
	}

	public void SetXBoard(int x)
	{ 
		xBoard = x; 
	}

	public void SetYBoard(int y)
	{
	 yBoard = y;
	}
	private void OnMouseUp() //Aka whenever a piece is clicked
	{
		if (!controller.GetComponent<Game>().IsGameOver() && controller.GetComponent<Game>().GetCurrentPlayer() == player)
		{
		DestroyMovePlates();  

		InitiateMovePlates();

		}

	}
	public void DestroyMovePlates()
	{
		GameObject[] movePlates = GameObject.FindGameObjectsWithTag("MovePlate");
		for (int i = 0; i < movePlates.Length; i++)
		{
			Destroy(movePlates[i]);
		}
	}

	public void InitiateMovePlates() //When a piece is clicked, this will figure out which moves are available to that piece.
	{
		switch (this.name)
		{
			case "black_queen":
			case "white_queen":
				LineMovePlate(1, 0);
				LineMovePlate(0, 1);
				LineMovePlate(1, 1);
				LineMovePlate(-1, 0);
				LineMovePlate(0, -1);
				LineMovePlate(-1, -1);
				LineMovePlate(-1, 1);
				LineMovePlate(1, -1);
				break;
			case "black_knight":
			case "white_knight":
				LMovePlate();
				break;
			case "black_bishop":
			case "white_bishop":
				LineMovePlate(1, 1);
				LineMovePlate(1, -1);
				LineMovePlate(-1, 1);
				LineMovePlate(-1, -1);
				break;
			case "black_King":
			case "white_King":
				SurroundMovePlate();
				break;
			case "black_rook":
			case "white_rook":
				LineMovePlate(1, 0);
				LineMovePlate(0, 1);
				LineMovePlate(-1, 0);
				LineMovePlate(0, -1);
				break;
			case "black_pawn":
				PawnMovePlate(xBoard, yBoard - 1);
				break;
			case "white_pawn":
				PawnMovePlate(xBoard, yBoard + 1);
				break;
		}
	}

	public void LineMovePlate(int xIncrement, int yIncrement)
	{
		Game sc = controller.GetComponent<Game>();

		int x = xBoard + xIncrement;
		int y = yBoard + yIncrement;

		while(sc.PositionOnBoard(x, y) && sc.GetPosition(x,y) == null)
		{
			MovePlateSpawn(x, y);
			x += xIncrement;
			y += yIncrement;
		}

		if (sc.PositionOnBoard(x, y) && sc.GetPosition(x, y).GetComponent<Chessman>().player != player) //checks to see if the piece in a moveplate is you or the opponent.
		{
			MovePlateAttackSpawn(x, y);
		}
	}
	public void LMovePlate()
	{
		PointMovePlate(xBoard + 1, yBoard + 2);
		PointMovePlate(xBoard - 1, yBoard + 2);
		PointMovePlate(xBoard + 2, yBoard + 1);
		PointMovePlate(xBoard + 2, yBoard - 1);
		PointMovePlate(xBoard + 1, yBoard - 2);
		PointMovePlate(xBoard - 1, yBoard - 2);
		PointMovePlate(xBoard - 2, yBoard + 1);
		PointMovePlate(xBoard - 2, yBoard - 1);
	}
	public bool canCastle()
	{
		Game sc = controller.GetComponent<Game>();
		GameObject cp = sc.GetPosition(xBoard, yBoard);
		switch (player)
		{
			case ("white"):
				if (cp.GetComponent<Chessman>().hasMoved == false && sc.GetPosition(0, 0).GetComponent<Chessman>().hasMoved == false &&
					sc.GetPosition(7, 0).GetComponent<Chessman>().hasMoved == false)
				{
					if (sc.GetPosition(xBoard - 3, yBoard) == null && sc.GetPosition(xBoard - 2, yBoard) == null && sc.GetPosition(xBoard - 2, yBoard) == null)
						return true;
					if (sc.GetPosition(xBoard + 1, yBoard) == null && sc.GetPosition(xBoard + 2, yBoard) == null)
						return true;
				}
				return false;
			case ("black"):
				if (cp.GetComponent<Chessman>().hasMoved == false && sc.GetPosition(0, 7).GetComponent<Chessman>().hasMoved == false &&
					sc.GetPosition(7, 7).GetComponent<Chessman>().hasMoved == false)
				{
					if (sc.GetPosition(xBoard - 3, yBoard) == null && sc.GetPosition(xBoard - 2, yBoard) == null && sc.GetPosition(xBoard - 2, yBoard) == null)
						return true;
					if (sc.GetPosition(xBoard + 1, yBoard) == null && sc.GetPosition(xBoard + 2, yBoard) == null)
						return true;
				}
				return false;
		}
		return false;
	}
	public void SurroundMovePlate()
	{
		PointMovePlate(xBoard, yBoard + 1);
		PointMovePlate(xBoard, yBoard - 1);
		PointMovePlate(xBoard - 1, yBoard - 1);
		PointMovePlate(xBoard - 1, yBoard - 0);
		PointMovePlate(xBoard - 1, yBoard + 1);
		PointMovePlate(xBoard + 1, yBoard - 1);
		PointMovePlate(xBoard + 1, yBoard - 0);
		PointMovePlate(xBoard + 1, yBoard + 1);

		if (canCastle())
		{
			MovePlateSpawn(xBoard - 3, yBoard);
			MovePlateSpawn(xBoard + 2, yBoard);
		}

	}
	public void PointMovePlate(int x, int y)
	{
		Game sc = controller.GetComponent<Game>();
		if (sc.PositionOnBoard(x,y)) 
		{
			GameObject cp = sc.GetPosition(x,y);

			if (cp == null)
			{
				MovePlateSpawn(x, y);
			}else if (cp.GetComponent<Chessman>().player != player)
			{
				MovePlateAttackSpawn(x, y);	
			}
		}
	}

	public void PawnMovePlate(int x, int y) //y is already set correctly for each pawns moves. Here we are gonna check for more cases
	{
		Game sc = controller.GetComponent<Game>();
		if ( y  == 2 || y == 5 || (y == 5 && player == "white") || (y == 2 && player == "black")) //I dont want to check special cases unless they are actually possible
			PawnSpecialChecks(x, y);
		if (sc.PositionOnBoard(x, y))
		{

			if (sc.GetPosition(x, y) == null) //Normal check after all special checks are completed
			{
				MovePlateSpawn(x, y);
			}
			if (sc.PositionOnBoard(x + 1, y) && sc.GetPosition(x+1, y) != null && sc.GetPosition(x+1, y).GetComponent<Chessman>().player != player)
			{
				MovePlateAttackSpawn(x+1, y);
			}

			if (sc.PositionOnBoard(x - 1, y) && sc.GetPosition(x - 1, y) != null && sc.GetPosition(x - 1, y).GetComponent<Chessman>().player != player)
			{
				MovePlateAttackSpawn(x - 1, y);
			}
		}
	}

	public void PawnSpecialChecks(int x, int y)
	{
		Game sc = controller.GetComponent<Game>();
		if (sc.PositionOnBoard(x, y))
		{
			//checks to see if the pawn can move 2 spaces for its initial move (white)
			if (sc.GetPosition(x, y) == null && sc.GetPosition(x, y + 1) == null)//checks where the piece is at, then both spaces in front.
			{
				MovePlateSpawn(x, y + 1);
			}
			else if (sc.GetPosition(x, y) == null && sc.GetPosition(x, y - 1) == null) //checks to see if the pawn can move 2 spaces for its initial move (black)
			{
				MovePlateSpawn(x, y - 1);
			}
			//En passant Checking. Should only occure if the pawns are in this part of the board!
			if ((y == 5 && player == "white") || (y == 2 && player == "black"))
			{
				switch (player)
				{
					case "white": //x - 1 is left side of pawn, x + 1 is right side.
						if (sc.PositionOnBoard(x - 1, y - 1) && sc.GetPosition(x - 1, y - 1) != null && sc.GetPosition(x - 1, y - 1).GetComponent<Chessman>().player != player)
						{
							if (sc.GetPosition(x - 1, y - 1).GetComponent<Chessman>().isEnPassantable)
							{
								MovePlateAttackSpawn(x - 1, y);
							}
						}
						if (sc.PositionOnBoard(x + 1, y - 1) && sc.GetPosition(x + 1, y - 1) != null && sc.GetPosition(x + 1, y - 1).GetComponent<Chessman>().player != player)
						{
							if (sc.GetPosition(x + 1, y - 1).GetComponent<Chessman>().isEnPassantable)
							{
								MovePlateAttackSpawn(x + 1, y);
							}
						}
						break;
					case "black":
						if (sc.PositionOnBoard(x - 1, y + 1) && sc.GetPosition(x - 1, y + 1) != null && sc.GetPosition(x - 1, y + 1).GetComponent<Chessman>().player != player)
						{
							if (sc.GetPosition(x - 1, y + 1).GetComponent<Chessman>().isEnPassantable)
							{
								MovePlateAttackSpawn(x - 1, y);
							}
						}
						if (sc.PositionOnBoard(x + 1, y + 1) && sc.GetPosition(x + 1, y + 1) != null && sc.GetPosition(x + 1, y + 1).GetComponent<Chessman>().player != player)
						{
							if (sc.GetPosition(x + 1, y + 1).GetComponent<Chessman>().isEnPassantable)
							{
								MovePlateAttackSpawn(x + 1, y);
							}
						}
						break;
				}
			}
		}
	}
	public void MovePlateSpawn(int matrixX, int matrixY) //sets the coordinates for where the move plate shows on the board
	{
		float x = matrixX;
		float y = matrixY;

		x *= 0.66f;
		y *= 0.66f;

		x += -2.3f;
		y += -2.3f;

		GameObject mp = Instantiate(movePlate, new Vector3(x, y, -3.0f), Quaternion.identity);

		MovePlate mpScript = mp.GetComponent<MovePlate>();
		mpScript.SetReference(gameObject);
		mpScript.SetCoords(matrixX, matrixY);
	}
	public void MovePlateAttackSpawn(int matrixX, int matrixY)
	{
		float x = matrixX;
		float y = matrixY;

		x *= 0.66f;
		y *= 0.66f;

		x += -2.3f;
		y += -2.3f;

		GameObject mp = Instantiate(movePlate, new Vector3(x, y, -3.0f), Quaternion.identity);

		MovePlate mpScript = mp.GetComponent<MovePlate>();
		mpScript.attack = true;
		mpScript.SetReference(gameObject);
		mpScript.SetCoords(matrixX, matrixY);
	}
}
