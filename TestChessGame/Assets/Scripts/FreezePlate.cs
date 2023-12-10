using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FreezePlate : MonoBehaviour
{

	public GameObject controller;
	GameObject reference = null;

	List<GameObject> coloredPlates = new List<GameObject>(); //This will keep track of the plates with colors changed.
	public static Vector2Int lastClickedPlateCoords = new Vector2Int(-1, -1); //This keeps the Dark blue plates until end of turn.

	//Board Posistions
	int matrixX;
	int matrixY;

	Dictionary<int, string> BoardPosLetter = new Dictionary<int, string>(8);

	//When the mouse hovers over the GameObject, it turns to this color
	Color freezeColor = new Color(0.0f, 0.0f, 0.8f, 1.0f); // Dark blue color

	//This stores the GameObject’s original color
	Color m_OriginalColor;

	//Get the GameObject’s mesh renderer to access the GameObject’s material and color
	SpriteRenderer s_Renderer;
	// Start is called before the first frame update
	void Start()
    {
		controller = GameObject.FindGameObjectWithTag("GameController");
		//Fetch the Sprite renderer component from the GameObject
		s_Renderer = GetComponent<SpriteRenderer>();
		//Fetch the original color of the GameObject
		m_OriginalColor = s_Renderer.material.color;
		BoardPosLetter.Add(0, "a");
		BoardPosLetter.Add(1, "b");
		BoardPosLetter.Add(2, "c");
		BoardPosLetter.Add(3, "d");
		BoardPosLetter.Add(4, "e");
		BoardPosLetter.Add(5, "f");
		BoardPosLetter.Add(6, "g");
		BoardPosLetter.Add(7, "h");
	}
	void Update()
	{
		HighlightArea(lastClickedPlateCoords);
	}

	public void OnMouseUp()
	{
		controller = GameObject.FindGameObjectWithTag("GameController");
		
		Game game = controller.GetComponent<Game>();

		string FrozenPos = BoardPosLetter[matrixX] + (matrixY).ToString();
		game.frozenPiece = FrozenPos;

	   Game sc = controller.GetComponent<Game>();
		int boardSize = 8; // Assuming an 8x8 chess board
		Spells spellsScript = reference.GetComponent<Spells>();
		int fIndex = 0; //I gotta use this to fill my arrays of frozen pieces.

		for (int x = matrixX - 1; x <= matrixX + 1; x++)
		{
			for (int y = matrixY - 1; y <= matrixY + 1; y++)
			{

				if (x >= 0 && x < boardSize && y >= 0 && y < boardSize)
				{
					GameObject plate = sc.freezePlates[x, y];
					if (plate != null)
					{
						GameObject piece = sc.GetPosition(x, y);
						if (piece != null)
						{
							piece.GetComponent<Chessman>().canMove = false;
							piece.GetComponent<SpriteRenderer>().material.color = freezeColor;
							if (sc.GetCurrentPlayer() == "white")
							{
								sc.GetComponent<Game>().frozenWhite[fIndex] = piece;
								fIndex += 1;
							}
							else
							{
								sc.GetComponent<Game>().frozenBlack[fIndex] = piece;
								fIndex += 1;
							}

						}
					}
				}
			}
		}
		spellsScript.DestroyFreezePlates();


		if (game.GetCurrentPlayer() == "white")
		{
			game.WFreezeSpells -= 1;
			game.lastOneTurnW = true;
			game.wCanUseSpell = 3;
		}
		else
		{
			game.BFreezeSpells -= 1;
			game.lastOneTurnB = true;
			game.bCanUseSpell = 3;
		}

	}
	void OnMouseOver()
	{
		//s_Renderer.material.color = m_MouseOverColor;
		controller = GameObject.FindGameObjectWithTag("GameController");

		Game sc = controller.GetComponent<Game>();
		int boardSize = 8; // Assuming an 8x8 chess board

		coloredPlates.Clear(); // Clear the list for new entries

		for (int x = matrixX - 1; x <= matrixX + 1; x++)
		{
			for (int y = matrixY - 1; y <= matrixY + 1; y++)
			{
				if (x >= 0 && x < boardSize && y >= 0 && y < boardSize)
				{
					GameObject plate = sc.freezePlates[x, y];
					if (plate != null)
					{
						plate.GetComponent<SpriteRenderer>().color = freezeColor;
						coloredPlates.Add(plate); // Add the plate to the list
					}
				}
			}
		}
	}
	void OnMouseExit()
	{
		foreach (GameObject plate in coloredPlates)
		{
			if (plate != null)
			{
				plate.GetComponent<SpriteRenderer>().color = m_OriginalColor;
			}
		}
		coloredPlates.Clear(); // Clear the list after resetting colors
	}
	public void SetReference(GameObject obj)
	{
		reference = obj;
	}
	public void SetCoords(int x, int y)
	{
		matrixX = x;
		matrixY = y;
	}
	public GameObject GetReference()
	{
		return reference;
	}
	void HighlightArea(Vector2Int centerCoords) //This is used in the Update() function to keep the area marked blue until the end of the turn.
	{
		if (centerCoords.x == -1 || centerCoords.y == -1)
		{
			return; // Exit if the last clicked coordinates are invalid
		}
		Game sc = controller.GetComponent<Game>();
		int boardSize = 8;

		for (int x = centerCoords.x - 1; x <= centerCoords.x + 1; x++)
		{
			for (int y = centerCoords.y - 1; y <= centerCoords.y + 1; y++)
			{
				if (x >= 0 && x < boardSize && y >= 0 && y < boardSize)
				{
					GameObject piece = sc.GetComponent<Game>().GetPosition(x, y);
					if (piece != null)
					{
						piece.GetComponent<SpriteRenderer>().color = freezeColor;

					}		
				}
			}
		}
	}
}
