using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JumpPlate : MonoBehaviour
{
	public GameObject controller;
	GameObject reference = null;

	List<GameObject> coloredPlates = new List<GameObject>(); //This will keep track of the plates with colors changed.

	Color jumpColor = new Color(0.0f, 0.0f, 0.8f, 1.0f); // Dark blue color TODO Change Color
	Color m_OriginalColor;
	Color j_OriginalColor = new Color(0.8f, 0.9f, 0.3f, 1.0f); //lime
	SpriteRenderer s_Renderer;

	//Board Posistions
	int matrixX;
	int matrixY;
	// Start is called before the first frame update
	void Start()
    {
		controller = GameObject.FindGameObjectWithTag("GameController");
		//Fetch the Sprite renderer component from the GameObject
		s_Renderer = GetComponent<SpriteRenderer>();
		//Fetch the original color of the GameObject
		m_OriginalColor = s_Renderer.material.color;
	}

    // Update is called once per frame
    void Update()
    {
        
    }

	public void OnMouseUp()
	{
		controller = GameObject.FindGameObjectWithTag("GameController");

		Game game = controller.GetComponent<Game>();

		Game sc = controller.GetComponent<Game>();
		Spells spellsScript = reference.GetComponent<Spells>();

		GameObject plate = sc.jumpPlates[matrixX, matrixY];
		if (plate != null)
		{
			GameObject piece = sc.GetPosition(matrixX, matrixY);
			if (piece != null)
			{
				piece.GetComponent<Chessman>().jumpable = true;
				piece.GetComponent<SpriteRenderer>().material.color = jumpColor;
			}
		}
		spellsScript.DestroyJumpPlates();
		if (game.GetCurrentPlayer() == "white")
		{
			game.WJumpSpells -= 1;
			game.wCanUseSpellj = 3;
		}
		else
		{
			game.BJumpSpells -= 1;
			game.bCanUseSpellj = 3;
		}
	}

	void OnMouseOver()
	{
		//s_Renderer.material.color = m_MouseOverColor;
		controller = GameObject.FindGameObjectWithTag("GameController");

		Game sc = controller.GetComponent<Game>();
		int boardSize = 8; // Assuming an 8x8 chess board

		coloredPlates.Clear(); // Clear the list for new entries

		if (matrixX >= 0 && matrixX < boardSize && matrixY >= 0 && matrixY < boardSize)
		{
			GameObject plate = sc.jumpPlates[matrixX, matrixY];
			if (plate != null)
			{
				plate.GetComponent<SpriteRenderer>().color = jumpColor;
				coloredPlates.Add(plate); // Add the plate to the list
			}
		}
	}

	private void OnMouseExit()
	{
		foreach (GameObject plate in coloredPlates)
		{
			if (plate != null)
			{
				plate.GetComponent<SpriteRenderer>().color = j_OriginalColor;
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
}
