using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovePlate : MonoBehaviour
{
	public GameObject controller;

	GameObject reference = null;

	//Board Posistions
	int matrixX;
	int matrixY;
	
	//false = movement, true = attacking
	public bool attack = false;

	public void Start()
	{
		if (attack)
		{
			gameObject.GetComponent<SpriteRenderer>().color = new Color(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}
	public void OnMouseUp()
	{
		controller = GameObject.FindGameObjectWithTag("GameController");
		Chessman MovingPiece = reference.GetComponent<Chessman>();
		GameObject SeperatePiece = null;
		GameObject cp = controller.GetComponent<Game>().GetPosition(matrixX, matrixY);

		int bPieces = controller.GetComponent<Game>().playerBlack.Length;
		int wPieces = controller.GetComponent<Game>().playerBlack.Length;
		
		if (attack)
		{
			
			if (cp == null) //only possible if its an en passant move
			{
				if (controller.GetComponent<Game>().GetCurrentPlayer() == "white")
				{
					cp = controller.GetComponent<Game>().GetPosition(matrixX, matrixY - 1); //Takes the pawn behind the piece
				}
				if (controller.GetComponent<Game>().GetCurrentPlayer() == "black")
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
		}
		if (MovingPiece.name == "white_King" || MovingPiece.name == "black_King") { //This is logic to perform the Castling move where king and rook move.
			if (System.Math.Abs(MovingPiece.GetXBoard() - matrixX) == 3)
			{
				SeperatePiece = controller.GetComponent<Game>().GetPosition(matrixX + 1, matrixY);
			}
			if(System.Math.Abs(MovingPiece.GetXBoard() - matrixX) == 2)
			{

			}
		}
		controller.GetComponent<Game>().SetPositionEmpty(MovingPiece.GetXBoard(),MovingPiece.GetYBoard()); //sets the Moving pieces current space to empty.

		MovingPiece.SetXBoard(matrixX);
		MovingPiece.SetYBoard(matrixY);
		MovingPiece.SetCoords();
		

		controller.GetComponent<Game>().SetPosition(reference);
		if(controller.GetComponent<Game>().GetCurrentPlayer() == "white") //This section just ensures that enpassant can only happen during the next turn.
		{
			for (int i = 0; i < bPieces; i++)
			{
				GameObject piece = controller.GetComponent<Game>().playerBlack[i];
				if (piece != null)
				{
					piece.GetComponent<Chessman>().isEnPassantable = false; //Very long, but just changes each piece to not be enpassantable after the oposing players move.
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
					piece.GetComponent<Chessman>().isEnPassantable = false; //Very long, but just changes each piece to not be enpassantable after the oposing players move.
				}
			}
		}
		if (MovingPiece.name == "black_King")
		{

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

