using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting.ReorderableList;
using UnityEditor.Experimental.GraphView;
using UnityEditor.U2D;
using UnityEngine;

public class MovePlate : MonoBehaviour
{
	
	public GameObject controller;

	GameObject reference = null;

	//Board Posistions
	int matrixX;
	int matrixY;

	Dictionary<int, string> BoardPosLetter = new Dictionary<int, string>(8);

	
	//false = movement, true = attacking
	public bool attack = false;

	public void Start()
	{
		if (attack)
		{
			gameObject.GetComponent<SpriteRenderer>().color = new Color(1.0f, 0.0f, 0.0f, 1.0f);
		}
		BoardPosLetter.Add(0, "a");
		BoardPosLetter.Add(1, "b");
		BoardPosLetter.Add(2, "c");
		BoardPosLetter.Add(3, "d");
		BoardPosLetter.Add(4, "e");
		BoardPosLetter.Add(5, "f");
		BoardPosLetter.Add(6, "g");
		BoardPosLetter.Add(7, "h");
	}
	public void OnMouseUp()
	{
		controller = GameObject.FindGameObjectWithTag("GameController");
		Chessman MovingPiece = reference.GetComponent<Chessman>();
		GameObject cp = controller.GetComponent<Game>().GetPosition(matrixX, matrixY);

		//Clicked pieces initial position
		int PieceX = reference.GetComponent<Chessman>().GetXBoard();
		int PieceY = reference.GetComponent<Chessman>().GetYBoard();

		string CurrentPlayer = controller.GetComponent<Game>().GetCurrentPlayer();

		int bPieces = controller.GetComponent<Game>().playerBlack.Length;
		int wPieces = controller.GetComponent<Game>().playerBlack.Length;
		
		if (attack)
		{
		
			if (cp == null) //only possible if its an en passant move
			{
				if (CurrentPlayer == "white")
				{
					cp = controller.GetComponent<Game>().GetPosition(matrixX, matrixY - 1); //Takes the pawn behind the piece
				}
				if (CurrentPlayer == "black")
				{
					cp = controller.GetComponent<Game>().GetPosition(matrixX, matrixY + 1); //Takes the pawn behind the piece
				}
			}

			if (cp.name == "white_King") controller.GetComponent<Game>().Winner("black");
			if (cp.name == "black_King") controller.GetComponent<Game>().Winner("white");

			Destroy(cp);
		}

		if (System.Math.Abs(MovingPiece.GetYBoard() - matrixY) > 1 //Sets En Passant in the case of moving 2 spaces for a pawn.
			&& (MovingPiece.name == "black_pawn" || MovingPiece.name == "white_pawn")) 
		{
			MovingPiece.isEnPassantable = true;
			string PiecePos = BoardPosLetter[matrixX] + (matrixY - 1).ToString();
			controller.GetComponent<Game>().enPasPos[0] = PiecePos;

		}
		if (MovingPiece.name == "black_pawn" || MovingPiece.name == "white_pawn")
		{
			switch(MovingPiece.name)
			{
				case "black_pawn":
					if(matrixY == 0)
					{
						GameObject thisPiece = controller.GetComponent<Game>().GetPosition(PieceX, PieceY);
						thisPiece.GetComponent<Chessman>().PromotePawn(reference, "black_queen", matrixX, matrixY);
						break;
					}
					break;
				case "white_pawn":
					if (matrixY == 7)
					{
						GameObject thisPiece = controller.GetComponent<Game>().GetPosition(PieceX, PieceY);
						thisPiece.GetComponent<Chessman>().PromotePawn(reference, "white_queen", matrixX, matrixY);
						break;
					}
					break;
			}
		}
		if (MovingPiece.name == "white_King" || MovingPiece.name == "black_King") { //This is logic to perform the Castling move where king and rook move. A lot is going on here!
			GameObject SeperatePiece;
			if (System.Math.Abs(MovingPiece.GetXBoard() - matrixX) == 3)
			{
				//These commands get the Rooks position, sets it too empty, then moves the rook to the new place.
				SeperatePiece = controller.GetComponent<Game>().GetPosition(matrixX - 1, matrixY);
				GameObject Rook = SeperatePiece.gameObject;
				controller.GetComponent<Game>().SetPositionEmpty(SeperatePiece.GetComponent<Chessman>().GetXBoard(), SeperatePiece.GetComponent<Chessman>().GetYBoard());
				SeperatePiece.GetComponent<Chessman>().SetXBoard(matrixX + 1); //These will change the position on the board
				SeperatePiece.GetComponent<Chessman>().SetYBoard(matrixY);
				SeperatePiece.GetComponent<Chessman>().SetCoords();
				controller.GetComponent<Game>().SetPosition(Rook); //This updates the games array for each piece, so that we can continue to keep track of it. 
			}
			if (System.Math.Abs(MovingPiece.GetXBoard() - matrixX) == 2)
			{
				SeperatePiece = controller.GetComponent<Game>().GetPosition(matrixX + 1, matrixY);
				GameObject Rook = SeperatePiece.gameObject;
				controller.GetComponent<Game>().SetPositionEmpty(SeperatePiece.GetComponent<Chessman>().GetXBoard(), SeperatePiece.GetComponent<Chessman>().GetYBoard());
				SeperatePiece.GetComponent<Chessman>().SetXBoard(matrixX - 1);
				SeperatePiece.GetComponent<Chessman>().SetYBoard(matrixY);
				SeperatePiece.GetComponent<Chessman>().SetCoords();
				controller.GetComponent<Game>().SetPosition(Rook);
			}
		}
		controller.GetComponent<Game>().SetPositionEmpty(MovingPiece.GetXBoard(),MovingPiece.GetYBoard()); //sets the Moving pieces current space to empty.

		MovingPiece.SetXBoard(matrixX);
		MovingPiece.SetYBoard(matrixY);
		MovingPiece.SetCoords();

		controller.GetComponent<Game>().SetPosition(reference);
		if(controller.GetComponent<Game>().GetCurrentPlayer() == "white" && (!controller.GetComponent<Game>().IsGameOver())) //This section just ensures that enpassant can only happen during the next turn.
		{
			for (int i = 0; i < bPieces; i++)
			{
				GameObject piece = controller.GetComponent<Game>().playerBlack[i];
				if (piece != null)
				{
					piece.GetComponent<Chessman>().isEnPassantable = false; //just changes each piece to not be enpassantable after the oposing players move.
				}
			}
		}
		if (controller.GetComponent<Game>().GetCurrentPlayer() == "black")
		{
			for (int i = 0; i < wPieces; i++)
			{
				GameObject piece = controller.GetComponent<Game>().playerWhite[i];
				if (piece != null)
				{
					piece.GetComponent<Chessman>().isEnPassantable = false; //just changes each piece to not be enpassantable after the oposing players move.
				}
			}
		}

		//These makes sure to mark Kings or Rooks as having moved, invalidating Castling for that piece. 
		if (MovingPiece.name == "black_King")
		{
			MovingPiece.hasMoved = true;
			controller.GetComponent<Game>().k = false;
			controller.GetComponent<Game>().q = false;
		}
		if (MovingPiece.name == "black_rook")
		{
			MovingPiece.hasMoved = true;
			print(matrixX);
		}
		if (MovingPiece.name == "white_King")
		{
			MovingPiece.hasMoved = true;
			controller.GetComponent<Game>().k = false;
			controller.GetComponent<Game>().q = false;
		}
		if (MovingPiece.name == "white_rook")
		{
			MovingPiece.hasMoved = true;
			print(matrixX);
		}

		controller.GetComponent<Game>().NextTurn();

		MovingPiece.DestroyMovePlates();
	}
	public void SetCoords(int x, int y)
	{
		matrixX = x;
		matrixY = y;
	}
	public void SetReference(GameObject obj)
	{
		reference = obj;
	}
	public GameObject GetReference()
	{
		return reference;
	}
}

