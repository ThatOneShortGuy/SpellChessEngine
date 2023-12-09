using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Spells : MonoBehaviour
{
	public GameObject controller;
	public GameObject freezePlate;
	public GameObject jumpPlate;


	private GameObject[,] openPositions = new GameObject[8, 8];

	GameObject reference = null;

	public bool wasPressed = false;

	private string player;
	// Start is called before the first frame update
	void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        
    }

	public void Activate()
	{
		controller = GameObject.FindGameObjectWithTag("GameController");
	}
	private void OnMouseUp()
	{
		Game game = controller.GetComponent<Game>();
		if (this.name == "whiteFreeze" && controller.GetComponent<Game>().GetCurrentPlayer() == "white" && game.wCanUseSpell == 0 && game.WFreezeSpells != 0)
		{
			if (this.wasPressed == false)
			{
				DestroyFreezePlates();
				InstantiateFreezePlates();
				this.wasPressed = true;
			}
			else
			{
				DestroyFreezePlates();
				this.wasPressed = false;
			}
		}
		if (this.name == "blackFreeze" && controller.GetComponent<Game>().GetCurrentPlayer() == "black" && game.bCanUseSpell == 0 && game.BFreezeSpells != 0) 
		{
			if (this.wasPressed == false)
			{
				DestroyFreezePlates();
				InstantiateFreezePlates();
				this.wasPressed = true;
			}
			else
			{
				DestroyFreezePlates();
				this.wasPressed = false;
			}
		}
		if (this.name == "whiteJump" && controller.GetComponent<Game>().GetCurrentPlayer() == "white" && game.wCanUseSpellj == 0 && game.WJumpSpells != 0)
		{
			if (this.wasPressed == false)
			{
				DestroyJumpPlates();
				InstantiateJumpPlates();
				this.wasPressed = true;
			}
			else
			{
				DestroyJumpPlates();
				this.wasPressed = false;
			}
		}
		if (this.name == "blackJump" && controller.GetComponent<Game>().GetCurrentPlayer() == "black" && game.bCanUseSpellj == 0 && game.BJumpSpells != 0) 
		{
			if (this.wasPressed == false)
			{
				DestroyJumpPlates();
				InstantiateJumpPlates();
				this.wasPressed = true;
			}
			else
			{
				DestroyJumpPlates();
				this.wasPressed = false;
			}
		}
	}
	public void DestroyFreezePlates()
	{
		Game game = controller.GetComponent<Game>();
		GameObject[] freezePlates = GameObject.FindGameObjectsWithTag("FreezePlate");
		for (int i = 0; i < freezePlates.Length; i++)
		{
			Destroy(freezePlates[i]);
		}

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (game.freezePlates[i, j] != null)
				{
					game.freezePlates[i, j] = null;   // Remove the reference from the array
				}
			}
		}
	}

	public void InstantiateFreezePlates()
	{
		Game sc = controller.GetComponent<Game>();
		Chessman cm = controller.GetComponent<Chessman>();
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				float x = i;
				float y = j;

				x *= 0.66f;
				y *= 0.66f;

				x += -2.3f;
				y += -2.3f;
				GameObject mp = Instantiate(freezePlate, new Vector3(x, y, -3.0f), Quaternion.identity);
				controller.GetComponent<Game>().freezePlates[i, j] = mp; //This stores the reference of each gameplate into an array t 

				FreezePlate mpScript = mp.GetComponent<FreezePlate>();
				mpScript.SetReference(gameObject);
				mpScript.SetCoords(i, j);
			}
		}
	}
	public void InstantiateJumpPlates()
	{
		Game sc = controller.GetComponent<Game>();
		Chessman cm = controller.GetComponent<Chessman>();
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				float x = i;
				float y = j;

				x *= 0.66f;
				y *= 0.66f;

				x += -2.3f;
				y += -2.3f;
				GameObject mp = Instantiate(jumpPlate, new Vector3(x, y, -3.0f), Quaternion.identity);
				controller.GetComponent<Game>().jumpPlates[i, j] = mp; //This stores the reference of each gameplate into an array

				JumpPlate mpScript = mp.GetComponent<JumpPlate>();
				mpScript.SetReference(gameObject);
				mpScript.SetCoords(i, j);
			}
		}
	}

	public void DestroyJumpPlates()
	{
		Game game = controller.GetComponent<Game>();
		GameObject[] jumpPlates = GameObject.FindGameObjectsWithTag("JumpPlate");
		for (int i = 0; i < jumpPlates.Length; i++)
		{
			Destroy(jumpPlates[i]);
		}

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (game.jumpPlates[i, j] != null)
				{
					game.jumpPlates[i, j] = null;   // Remove the reference from the array
				}
			}
		}
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
