use crate::class::Class;
use crate::document::Document;
use crate::element::Element;

use std::io::Write;

struct TOCEntry<'a> {
    pub file_name: String,
    pub class: &'a Class,
}

impl<'a> TOCEntry<'a> {
    pub fn new(file_name: &str, class: &'a Class) -> Self {
        Self {
            file_name: file_name.to_string(),
            class,
        }
    }
}

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

        let mut table_of_contents = Vec::<TOCEntry>::new();
        for class in &self.classes {
            if !class.description().is_empty() {
                let mut class_path = path.join(class.name());
                class_path.set_extension("md");

                if let Some(os_file_name) = class_path.file_name() {
                    if let Some(file_name) = os_file_name.to_str() {
                        table_of_contents.push(TOCEntry::new(file_name, &class));
                    }
                }
    
                if let Ok(class_file) = std::fs::OpenOptions::new().write(true).truncate(true).create(true).open(&class_path) {
                    if let Err(error) = class.write(&class_file) {
                        println!("Failed to write class {}: {}", class.name(), error);
                    }
                }
            }
        }

        if let Err(error) = self.write_toc(table_of_contents, &path) {
            println!("Failed to write table of contents: {}", error);
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

    fn write_toc(&self, table_of_contents: Vec<TOCEntry>, root: &std::path::Path) -> Result<(), std::io::Error> {
        let mut toc_path = root.join("TOC");
        toc_path.set_extension("md");

        let mut marked_entries = Vec::<String>::new();
        if let Ok(toc_file) = std::fs::OpenOptions::new().write(true).truncate(true).create(true).open(&toc_path) {
            let mut writer = std::io::BufWriter::new(toc_file);

            writeln!(writer, "# Table of Contents\n")?;

            for item in &table_of_contents {
                // TODO: Pass into a recursive function with a depth.
                // Each entry should be looked over and see if its parent is this. If so, add the entry and repeat.
                // Marked entries should be returned and added to a handled list so that they are not repeated.
                if !Self::has_parent_entry(item, &table_of_contents) {
                    if !marked_entries.contains(&item.file_name) {
                        marked_entries.append(&mut Self::write_toc_entry(item, 0, &table_of_contents, &mut writer));
                    }
                }
            }
        }

        Ok(())
    }

    fn has_parent_entry(entry: &TOCEntry, toc: &Vec<TOCEntry>) -> bool {
        for item in toc {
            if entry.class.parent_name() == item.class.full_name() {
                return true;
            }
        }

        false
    }

    fn write_toc_entry(entry: &TOCEntry, depth: usize, toc: &Vec<TOCEntry>, writer: &mut std::io::BufWriter<std::fs::File>) -> Vec<String> {
        let mut result = Vec::<String>::new();

        result.push(entry.file_name.clone());
        if depth > 0 {
            if let Err(error) = write!(writer, "{:depth$}", "\t", depth=depth) {
                println!("Failed to write depth: {}", error);
            }
        }

        if let Err(error) = writeln!(writer, "* [{}]({})", entry.file_name.trim_end_matches(".md"), entry.file_name) {
            println!("Failed to write TOC entry {}: {}", entry.file_name, error);
        }

        for item in toc {
            if item.class.parent_name() == entry.class.full_name() {
                result.append(&mut Self::write_toc_entry(item, depth + 1, toc, writer));
            }
        }

        result
    }
}
