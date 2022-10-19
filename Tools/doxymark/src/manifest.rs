use crate::class::Class;
use crate::document::Document;
use crate::element::Element;

pub struct Manifest {
    root: String,
    classes: Vec<Class>,
}

impl Manifest {
    pub fn load(root: &str) -> Option<Self> {
        let mut result = Self {
            root: root.to_string(),
            classes: Vec::<Class>::new(),
        };

        if !result.parse_index() {
            return None;
        }

        Some(result)
    }

    pub fn generate(&self, directory: &str) -> bool {
        let path = std::path::Path::new(directory).join("md");
        if !path.exists() {
            let result = std::fs::create_dir_all(&path);

            match result {
                Err(error) => {
                    println!("Failed to create directory {}: {}", path.display(), error);
                    return false;
                }
                _ => {}
            }
        }

        for class in &self.classes {
            let mut class_path = path.join(class.name());
            class_path.set_extension("md");

            if let Ok(class_file) = std::fs::File::create(&class_path) {
                if let Err(error) = class.write(&class_file) {
                    println!("Failed to write class {}: {}", class.name(), error);
                }
            }
        }

        true
    }

    fn parse_index(&mut self) -> bool {
        let index_path = std::path::Path::new(&self.root).join("index.xml");
        if !index_path.exists() {
            println!("File {} not found!", index_path.display());
            return false;
        }

        if let Some(document) = Document::load(&index_path) {
            if let Some(index) = document.get_element("doxygenindex") {
                let compounds: Vec<&Element> = index.get_elements("compound");
    
                for compound in compounds {
                    if compound.get_attribute("kind") == "class" {
                        if let Some(name) = compound.get_element("name") {
                            let mut class_path = std::path::Path::new(&self.root).join(compound.get_attribute("refid"));
                            class_path.set_extension("xml");

                            let mut class = Class::new();
                            class.set_full_name(&name.get_inner());
                            class.path = class_path.to_str().unwrap().to_string();
                            self.classes.push(class);
                        }
                    }
                }
            }
        }

        println!("Parsed {} classes.", self.classes.len());
        for class in &mut self.classes {
            class.parse();
        }

        true
    }
}
